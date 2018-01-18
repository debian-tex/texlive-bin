/*************************************************************************
** StreamInputBufferTest.cpp                                            **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2018 Martin Gieseking <martin.gieseking@uos.de>   **
**                                                                      **
** This program is free software; you can redistribute it and/or        **
** modify it under the terms of the GNU General Public License as       **
** published by the Free Software Foundation; either version 3 of       **
** the License, or (at your option) any later version.                  **
**                                                                      **
** This program is distributed in the hope that it will be useful, but  **
** WITHOUT ANY WARRANTY; without even the implied warranty of           **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         **
** GNU General Public License for more details.                         **
**                                                                      **
** You should have received a copy of the GNU General Public License    **
** along with this program; if not, see <http://www.gnu.org/licenses/>. **
*************************************************************************/

#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include "InputBuffer.hpp"
#include "InputReader.hpp"

using std::istringstream;
using std::string;
using std::unordered_map;

TEST(StreamInputBufferTest, get) {
	istringstream iss("abcdefghijklmnopqrstuvwxyz");
	StreamInputBuffer buffer(iss, 10);
	BufferInputReader in(buffer);
	bool ok=true;
	for (int i=0; !in.eof() && ok; i++) {
		EXPECT_LT(i, 26);
		EXPECT_EQ(in.get(), 'a'+i);
		ok = (i < 26);
	}
}


TEST(StreamInputBufferTest, peek) {
	istringstream iss("abcdefghijklmnopqrstuvwxyz");
	StreamInputBuffer buffer(iss, 10);
	BufferInputReader in(buffer);
	EXPECT_EQ(in.peek(), 'a');
	for (int i=0; i < 20; i++)
		EXPECT_EQ(in.peek(i), 'a'+i);
	// we can't look forward more than BUFSIZE characters (10 in this case)
	for (int i=21; i < 26; i++)
		EXPECT_EQ(in.peek(i), -1);
}


TEST(StreamInputBufferTest, check) {
	istringstream iss("abcdefghijklmnopqrstuvwxyz");
	StreamInputBuffer buffer(iss, 10);
	BufferInputReader in(buffer);
	EXPECT_TRUE(in.check("abc", false));
	EXPECT_TRUE(in.check("abc", true));
	EXPECT_TRUE(in.check("def", true));
	EXPECT_TRUE(in.check("ghi", true));
	EXPECT_TRUE(in.check("jkl", true));
	EXPECT_TRUE(in.check("mnopqrst", false));
	EXPECT_TRUE(in.check("mnopqrst", true));
	EXPECT_TRUE(in.check("uvwxyz", true));
}


TEST(StreamInputBufferTest, skip) {
	istringstream iss("abcdefghijklmnopqrstuvwxyz");
	StreamInputBuffer buffer(iss, 10);
	BufferInputReader in(buffer);
	in.skip(3);
	EXPECT_EQ(in.peek(), 'd');
	in.skipUntil("ijk");
	EXPECT_EQ(in.peek(), 'l');
	in.skipUntil("z");
	EXPECT_TRUE(in.eof());
}


TEST(StreamInputBufferTest, parseInt) {
	istringstream iss("1234,-5,+6,10.-");
	StreamInputBuffer buffer(iss, 10);
	BufferInputReader in(buffer);
	int n;
	EXPECT_TRUE(in.parseInt(n));
	EXPECT_EQ(n, 1234);
	EXPECT_EQ(in.get(), ',');

	EXPECT_TRUE(in.parseInt(n));
	EXPECT_EQ(n, -5);
	EXPECT_EQ(in.get(), ',');

	EXPECT_TRUE(in.parseInt(n));
	EXPECT_EQ(n, 6);
	EXPECT_EQ(in.get(), ',');

	EXPECT_TRUE(in.parseInt(n));
	EXPECT_EQ(n, 10);
	EXPECT_EQ(in.get(), '.');

	EXPECT_FALSE(in.parseInt(n));
	EXPECT_EQ(in.get(), '-');
}


TEST(StreamInputBufferTest, parseUInt_base) {
	istringstream iss("1234,-5,10,1abc,1234a");
	StreamInputBuffer buffer(iss, 10);
	BufferInputReader in(buffer);
	unsigned n;
	EXPECT_TRUE(in.parseUInt(10, n));
	EXPECT_EQ(n, 1234u);
	EXPECT_EQ(in.get(), ',');

	EXPECT_FALSE(in.parseUInt(10, n));
	in.get();
	EXPECT_TRUE(in.parseUInt(10, n));
	EXPECT_EQ(n, 5u);
	EXPECT_EQ(in.get(), ',');

	EXPECT_TRUE(in.parseUInt(16, n));
	EXPECT_EQ(n, 16u);
	EXPECT_EQ(in.get(), ',');

	EXPECT_TRUE(in.parseUInt(16, n));
	EXPECT_EQ(n, 0x1ABCu);
	EXPECT_EQ(in.get(), ',');

	EXPECT_TRUE(in.parseUInt(8, n));
	EXPECT_EQ(n, 01234u);
	EXPECT_EQ(in.get(), 'a');
}


TEST(StreamInputBufferTest, parseDouble) {
	istringstream iss("1234,-5,6.12,-3.1415,-0.5,-.1,12e2,10.-");
	StreamInputBuffer buffer(iss, 10);
	BufferInputReader in(buffer);
	double d;
	EXPECT_EQ(in.parseDouble(d), 'i');
	EXPECT_EQ(d, 1234.0);
	EXPECT_EQ(in.get(), ',');

	EXPECT_EQ(in.parseDouble(d), 'i');
	EXPECT_EQ(d, -5.0);
	EXPECT_EQ(in.get(), ',');

	EXPECT_EQ(in.parseDouble(d), 'f');
	EXPECT_EQ(d, 6.12);
	EXPECT_EQ(in.get(), ',');

	EXPECT_EQ(in.parseDouble(d), 'f');
	EXPECT_EQ(d, -3.1415);
	EXPECT_EQ(in.get(), ',');

	EXPECT_EQ(in.parseDouble(d), 'f');
	EXPECT_EQ(d, -0.5);
	EXPECT_EQ(in.get(), ',');

	EXPECT_EQ(in.parseDouble(d), 'f');
	EXPECT_EQ(d, -0.1);
	EXPECT_EQ(in.get(), ',');

	EXPECT_EQ(in.parseDouble(d), 'f');
	EXPECT_EQ(d, 1200);
	EXPECT_EQ(in.get(), ',');

	EXPECT_EQ(in.parseDouble(d), 'f');
	EXPECT_EQ(d, 10.0);
	EXPECT_EQ(in.peek(), '-');

	EXPECT_FALSE(in.parseDouble(d));
	EXPECT_EQ(in.get(), '-');
}


TEST(StreamInputBufferTest, attribs) {
	istringstream iss("aaa=1 bbb=2 ccc=3 d e");
	StreamInputBuffer buffer(iss, 10);
	BufferInputReader in(buffer);
	unordered_map<string,string> attr;
	int s = in.parseAttributes(attr);
	EXPECT_EQ(s, 3);
	EXPECT_EQ(attr["aaa"], "1");
	EXPECT_EQ(attr["bbb"], "2");
	EXPECT_EQ(attr["ccc"], "3");
}


TEST(StreamInputBufferTest, invalidate) {
	istringstream iss("aaa=1 bbb=2 ccc=3 d e");
	StreamInputBuffer buffer(iss, 10);
	EXPECT_EQ(buffer.get(), 'a');
	EXPECT_EQ(buffer.get(), 'a');
	EXPECT_EQ(buffer.get(), 'a');
	EXPECT_EQ(buffer.get(), '=');
	buffer.invalidate();
	EXPECT_TRUE(buffer.eof());
}


TEST(StreamInputBufferTest, find) {
	istringstream iss("abcd efgh ijklmn abc");
	StreamInputBuffer buffer(iss);
	BufferInputReader reader(buffer);
	EXPECT_EQ(reader.find('x'), -1);
	EXPECT_EQ(reader.find('c'), 2);
	EXPECT_EQ(reader.find(' '), 4);
}


TEST(StreamInputBufferTest, getString) {
	istringstream iss("abcd efgh \"ijklm\"n abcdef 01234");
	StreamInputBuffer buffer(iss);
	BufferInputReader reader(buffer);
	EXPECT_EQ(reader.getString(), "abcd");
	EXPECT_EQ(reader.getString(), "efgh");
	EXPECT_EQ(reader.getQuotedString('"'), "ijklm");
	EXPECT_EQ(reader.getQuotedString('"'), "");
	EXPECT_EQ(reader.getString(4), "n ab");
	EXPECT_EQ(reader.getQuotedString(0), "cdef");
}

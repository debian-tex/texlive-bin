/*************************************************************************
** FilePathTest.cpp                                                     **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2013 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <string>
#include "FilePath.h"
#include "FileSystem.h"

using namespace std;


TEST(FilePathTest, dir1) {
	FilePath fp("a/b/c/d", false, "/");
	ASSERT_EQ(fp.absolute(), "/a/b/c/d");
	ASSERT_EQ(fp.relative("/"), "a/b/c/d");
	ASSERT_EQ(fp.relative("/a/b"), "c/d");
	ASSERT_EQ(fp.relative("/a/b/c"), "d");
	ASSERT_EQ(fp.relative("/a/b/c/d"), ".");
	ASSERT_EQ(fp.relative("/a/b/x"), "../c/d");
	ASSERT_EQ(fp.relative("/a/b/x/y"), "../../c/d");
}


TEST(FilePathTest, dir2) {
	FilePath fp("a/b/c/d", false, "/x/y");
	ASSERT_EQ(fp.absolute(), "/x/y/a/b/c/d");
	ASSERT_EQ(fp.relative("/"), "x/y/a/b/c/d");
	ASSERT_EQ(fp.relative("/x/y/a/b"), "c/d");
	ASSERT_EQ(fp.relative("/x/y/a/b/c"), "d");
	ASSERT_EQ(fp.relative("/x/y/a/b/c/d"), ".");
	ASSERT_EQ(fp.relative("/x/y/a/b/x"), "../c/d");
	ASSERT_EQ(fp.relative("/x/y/a/b/x/y"), "../../c/d");
}


TEST(FilePathTest, file1) {
	FilePath fp("a/b/c/d/f.ext", true, "/");
	ASSERT_EQ(fp.absolute(), "/a/b/c/d/f.ext");
	ASSERT_EQ(fp.relative("/"), "a/b/c/d/f.ext");
	ASSERT_EQ(fp.relative("/a/b"), "c/d/f.ext");
	ASSERT_EQ(fp.relative("/a/b/c"), "d/f.ext");
	ASSERT_EQ(fp.relative("/a/b/c/d"), "f.ext");
	ASSERT_EQ(fp.relative("/a/b/x"), "../c/d/f.ext");
	ASSERT_EQ(fp.relative("/a/b/x/y"), "../../c/d/f.ext");
	ASSERT_EQ(fp.basename(), "f");
	ASSERT_EQ(fp.suffix(), "ext");
	fp.suffix("new");
	ASSERT_EQ(fp.suffix(), "new");
	ASSERT_EQ(fp.relative("/a/b/x/y"), "../../c/d/f.new");
}


TEST(FilePathTest, file2) {
	FilePath fp("/f.ext", true, "/");
	ASSERT_EQ(fp.absolute(), "/f.ext");
	ASSERT_EQ(fp.relative("/a/b"), "../../f.ext");
}


#ifdef __WIN32__
static string tolower (const string &str) {
	string ret;
	for (string::const_iterator it=str.begin(); it != str.end(); ++it)
		ret += tolower(*it);
	return ret;
}
#endif


TEST(FilePathTest, autodetect) {
	FilePath fp1("FilePathTest.cpp");
	ASSERT_TRUE(fp1.isFile());
	ASSERT_FALSE(fp1.empty());
	string cwd = FileSystem::getcwd();
#ifdef __WIN32__
	ASSERT_EQ(fp1.absolute(), tolower(cwd + "/FilePathTest.cpp"));
#else
	ASSERT_EQ(fp1.absolute(), cwd + "/FilePathTest.cpp");
#endif

	FilePath fp2("");
	ASSERT_FALSE(fp2.isFile());
	ASSERT_FALSE(fp2.empty());
#ifdef __WIN32__
	ASSERT_EQ(fp2.absolute(), tolower(FileSystem::getcwd()));
#else
	ASSERT_EQ(fp2.absolute(), FileSystem::getcwd());
#endif
}


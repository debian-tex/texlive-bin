/*************************************************************************
** InputReader.cpp                                                      **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2014 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <config.h>
#include <cmath>
#include <vector>
#include "InputReader.h"

using namespace std;


/** Skips n characters. */
void InputReader::skip (size_t n) {
	while (n-- > 0)
		get();
}


/** Moves the buffer pointer to the next non-space character. A following call
 *  of get() returns this character. */
void InputReader::skipSpace () {
	while (isspace(peek()))
		get();
}


/** Tries to find a given string and skips all characters preceding that string.
 *  @param[in] s string to look for (must not be longer than the maximal buffer size)
 *  @param[in] consume if true, the buffer pointer is moved to the first character following string s
 *  @return true if s was found */
bool InputReader::skipUntil (const char *s, bool consume) {
	bool found = false;
	while (!eof() && !(found = check(s, consume)))
		get();
	return found;
}


/** Looks for the first occurrence of a given character.
 *  @param[in] c character to lookup
 *  @return position of character relative to current location, -1 if character was not found */
int InputReader::find (char c) const {
	int pos = 0;
	int cc;
	while ((cc = peek(pos)) >= 0 && cc != c)
		pos++;
	return cc < 0 ? -1 : pos;
}


/** Checks if the next characters to be read match a given string.
 *  @param[in] s string to be matched
 *  @param[in] consume if true, the characters of the matched string are skipped
 *  @return true if s matches */
bool InputReader::check (const char *s, bool consume) {
	size_t count = 0;
	for (const char *p=s; *p; p++) {
		if (peek(count++) != *p)
			return false;
	}
	if (consume)
		skip(count);
	return true;
}


int InputReader::compare (const char *s, bool consume) {
	size_t count = 0;
	for (const char *p=s; *p; p++) {
		int c = peek(count++);
		if (c != *p)
			return *p < c ? -1 : 1;
	}
	if (consume)
		skip(count);
	return 0;
}


/** Reads an integer from the buffer. All characters that are part of
 *  the read integer constant are skipped. If this function returns false,
 *  the buffer pointer points to the same position as before the function call.
 *  @param[out] val contains the read integer value on success
 *  @param[in] accept_sign if false, only positive integers (without sign) are accepted
 *  @return true if integer could be read */
bool InputReader::parseInt (int &val, bool accept_sign) {
	val = 0;
	int fac=1;
	char sign;    // explicitly given sign
	if (accept_sign && ((sign = peek()) == '+' || sign == '-')) {
		if (isdigit(peek(1))) {
			get();  // skip sign
			if (sign == '-')
				fac = -1;
		}
		else
			return false;
	}
	else if (!isdigit(peek()))
		return false;

	while (isdigit(peek()))
		val = val*10 + (get()-'0');
	val *= fac;
	return true;
}


bool InputReader::parseUInt (unsigned &val) {
	val = 0;
	if (!isdigit(peek()))
		return false;
	while (isdigit(peek()))
		val = val*10 + (get()-'0');
	return true;
}


bool InputReader::parseInt (int base, int &val) {
	if (base < 2 || base > 32)
		return false;

	const char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
	const char maxdigit = digits[base-1];
	char c;
	if (!isalnum(c = tolower(peek())) || c > maxdigit)
		return false;

	val = 0;
	while (isalnum(c = tolower(peek())) && c <= maxdigit) {
		get();
		int digit = c - (c <= '9' ? '0' : 'a'-10);
		val = val*base + digit;
	}
	return true;
}


/** Reads a double from the buffer. All characters that are part of
 *  the read double constant are skipped. If this function returns false,
 *  the buffer pointer points to the same position as before the function call.
 *  @param[out] val contains the read double value on success
 *  @return number details: 0=no number, 'i'=integer, 'f'=floating point number */
char InputReader::parseDouble (double &val) {
	int fac=1;
	int int_part=0;
	bool is_float = false;
	skipSpace();
	char sign = peek();
	if (parseInt(int_part)) { // match [+-]?[0-9]+\.?
		if (peek() == '.') {
			get();
			is_float = true;
		}
		if (int_part < 0 || sign == '-') {
			fac = -1;
			int_part = -int_part;
		}
	}
	else {  // match [+-]?\.
		char sign;   // explicitly given sign
		if ((sign = peek()) == '+' || sign == '-') { // match [+-]?\.[0-9]
			if (peek(1) != '.' || !isdigit(peek(2)))
				return 0;
			if (sign == '-')
				fac = -1;
			skip(2);  // skip sign and dot
		}
		else if (peek() == '.' && isdigit(peek(1)))
			get();
		else
			return 0;
		is_float = true;
	}
	// parse fractional part
	double frac_part=0.0;
	for (double u=10; isdigit(peek()); u*=10)
		frac_part += (get()-'0')/u;
	val = (int_part + frac_part) * fac;
	// parse exponent
	char c;
	if (tolower(peek()) == 'e' && (isdigit(c=peek(1)) || ((c == '+' || c == '-') && isdigit(peek(2))))) {
		get(); // skip 'e'
		int exp;
		parseInt(exp);
		val *= pow(10.0, exp);
		is_float = true;
	}
	return is_float ? 'f' : 'i';
}


/** Reads an integer value from the buffer. If no valid integer constant
 *  could be found at the current position 0 is returned. */
int InputReader::getInt () {
	skipSpace();
	int val;
	return parseInt(val) ? val : 0;
}


/** Reads an double value from the buffer. If no valid double constant
 *  could be found at the current position 0 is returned. */
double InputReader::getDouble () {
	skipSpace();
	double val;
	return parseDouble(val) ? val : 0.0;
}


string InputReader::getWord () {
	string ret;
	skipSpace();
	while (isalpha(peek()))
		ret += get();
	return ret;
}


char InputReader::getPunct () {
	skipSpace();
	if (ispunct(peek()))
		return get();
	return 0;
}


/** Reads a string delimited by a given quotation character.
 *  Before reading the string, all leading whitespace is skipped. Then, the function checks
 *  for the given quotation character. If it is found, all characters until the second
 *  appearance of the quotation char are appended to the result. Otherwise, an empty string
 *  is returned. If the quotation character is 0, the behavior of this function is identical to
 *  a call of getString().
 *  @param[in] quotechar the quotation character bounding the string to be read
 *  @return the string read */
string InputReader::getQuotedString (char quotechar) {
	if (quotechar == 0)
		return getString();

	string ret;
	skipSpace();
	if (peek() == quotechar) {
		get();
		while (!eof() && peek() != quotechar)
			ret += get();
		get();
	}
	return ret;
}


/** Reads a string delimited by whitespace and/or invisible characters.
 *  Before reading the string, all leading whitespace is skipped. Then, the function adds
 *  all printable characters to the result until a whitespace, an unprintable character, or
 *  EOF is found.
 *  @return the string read */
string InputReader::getString () {
	string ret;
	skipSpace();
	while (!eof() && !isspace(peek()) && isprint(peek()))
		ret += get();
	return ret;
}


/** Reads a given number of characters and returns the resulting string.
 *  @param n number of character to read
 *  @return the string read */
string InputReader::getString (size_t n) {
	string ret;
	while (n-- > 0)
		ret += get();
	return ret;
}


/** Parses a sequence of key-value pairs of the form KEY=VALUE or KEY="VALUE"
 *  @param[out] attr the scanned atributes
 *  @param[in] quotechar quote character used to enclose the attribute values
 *  @return number of attributes scanned */
int InputReader::parseAttributes (map<string,string> &attr, char quotechar) {
	bool ready=false;
	while (!eof() && !ready) {
		string key;
		skipSpace();
		while (isalnum(peek()))
			key += get();
		skipSpace();
		if (peek() == '=') {
			get();
			skipSpace();
			string val = getQuotedString(quotechar);
			attr[key] = val;
		}
		else
			ready = true;
	}
	return attr.size();
}

//////////////////////////////////////////


int StreamInputReader::peek (size_t n) const {
	if (n == 0)
		return peek();
	vector<char> chars(n);
	_is.read(&chars[0], n);
	int ret = peek();
	for (int i=n-1; i >= 0; i--)
		_is.putback(chars[i]);
	return ret;
}

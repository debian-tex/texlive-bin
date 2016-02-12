/*************************************************************************
** SVGOutput.cpp                                                        **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2016 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "gzstream.h"
#include "Calculator.h"
#include "FileSystem.h"
#include "Message.h"
#include "SVGOutput.h"


using namespace std;

SVGOutput::SVGOutput (const char *base, string pattern, int zipLevel)
	: _path(base ? base : ""),
	_pattern(pattern),
	_stdout(base == 0),
	_zipLevel(zipLevel),
	_page(-1),
	_os(0)
{
}


/** Returns an output stream for the given page.
 *  @param[in] page number of current page
 *  @param[in] numPages total number of pages in the DVI file
 *  @return output stream for the given page */
ostream& SVGOutput::getPageStream (int page, int numPages) const {
	string fname = filename(page, numPages);
	if (fname.empty()) {
		delete _os;
		_os = 0;
		return cout;
	}
	if (page == _page)
		return *_os;

	_page = page;
	delete _os;
	if (_zipLevel > 0)
		_os = new ogzstream(fname.c_str(), _zipLevel);
	else
		_os = new ofstream(fname.c_str());
	if (!_os || !*_os) {
		delete _os;
		_os = 0;
		throw MessageException("can't open file "+fname+" for writing");
	}
	return *_os;
}


/** Returns the name of the SVG file containing the given page.
 *  @param[in] page number of current page
 *  @param[in] numPages total number of pages */
string SVGOutput::filename (int page, int numPages) const {
	if (_stdout)
		return "";
	string pattern = _pattern;
	expandFormatString(pattern, page, numPages);
	// remove leading and trailing whitespace
	stringstream trim;
	trim << pattern;
	pattern.clear();
	trim >> pattern;
	// set and expand default pattern if necessary
	if (pattern.empty()) {
		pattern = numPages > 1 ? "%f-%p" : "%f";
		expandFormatString(pattern, page, numPages);
	}
	// append suffix if necessary
	FilePath outpath(pattern, true);
	if (outpath.suffix().empty())
		outpath.suffix(_zipLevel > 0 ? "svgz" : "svg");
	string abspath = outpath.absolute();
	string relpath = outpath.relative();
	return abspath.length() < relpath.length() ? abspath : relpath;
}


static int ilog10 (int n) {
	int result = 0;
	while (n >= 10) {
		result++;
		n /= 10;
	}
	return result;
}


/** Replace expressions in a given string by the corresponing values.
 *  Supported constructs:
 *  %f: basename of the current file (filename without suffix)
 *  %[0-9]?p: current page number
 *  %[0-9]?P: number of pages in DVI file
 *  %[0-9]?(expr): arithmetic expression */
void SVGOutput::expandFormatString (string &str, int page, int numPages) const {
	string result;
	while (!str.empty()) {
		size_t pos = str.find('%');
		if (pos == string::npos) {
			result += str;
			str.clear();
		}
		else {
			result += str.substr(0, pos);
			str = str.substr(pos);
			pos = 1;
			ostringstream oss;
			if (isdigit(str[pos])) {
				oss << setw(str[pos]-'0') << setfill('0');
				pos++;
			}
			else {
				oss << setw(ilog10(numPages)+1) << setfill('0');
			}
			switch (str[pos]) {
				case 'f':
					result += _path.basename();
					break;
				case 'p':
				case 'P':
					oss << (str[pos] == 'p' ? page : numPages);
					result += oss.str();
					break;
				case '(': {
					size_t endpos = str.find(')', pos);
					if (endpos == string::npos)
						throw MessageException("missing ')' in filename pattern");
					else if (endpos-pos-1 > 1) {
						try {
							Calculator calculator;
							calculator.setVariable("p", page);
							calculator.setVariable("P", numPages);
							oss << floor(calculator.eval(str.substr(pos, endpos-pos+1)));
							result += oss.str();
						}
						catch (CalculatorException &e) {
							oss.str("");
							oss << "error in filename pattern (" << e.what() << ")";
							throw MessageException(oss.str());
						}
						pos = endpos;
					}
					break;
				}
			}
			str = str.substr(pos+1);
		}
	}
	str = result;
}

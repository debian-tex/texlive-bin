/*************************************************************************
** JFM.cpp                                                              **
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

#include <config.h>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <sstream>
#include "JFM.h"
#include "StreamReader.h"

using namespace std;


JFM::JFM (istream &is) {
	is.seekg(0);
	StreamReader sr(is);
	UInt16 id = UInt16(sr.readUnsigned(2)); // JFM ID (9 or 11)
	if (id != 9 && id != 11) {
		ostringstream oss;
		oss << "invalid JFM identifier " << id << " (9 or 11 expected)";
		throw FontMetricException(oss.str());
	}
	_vertical = (id == 9);
	UInt16 nt = UInt16(sr.readUnsigned(2)); // length of character type table
	UInt16 lf = UInt16(sr.readUnsigned(2)); // length of entire file in 4 byte words
	UInt16 lh = UInt16(sr.readUnsigned(2)); // length of header in 4 byte words
	UInt16 bc = UInt16(sr.readUnsigned(2)); // smallest character code in font
	UInt16 ec = UInt16(sr.readUnsigned(2)); // largest character code in font
	UInt16 nw = UInt16(sr.readUnsigned(2)); // number of words in width table
	UInt16 nh = UInt16(sr.readUnsigned(2)); // number of words in height table
	UInt16 nd = UInt16(sr.readUnsigned(2)); // number of words in depth table
	UInt16 ni = UInt16(sr.readUnsigned(2)); // number of words in italic corr. table
	UInt16 nl = UInt16(sr.readUnsigned(2)); // number of words in glue/kern table
	UInt16 nk = UInt16(sr.readUnsigned(2)); // number of words in kern table
	UInt16 ng = UInt16(sr.readUnsigned(2)); // number of words in glue table
	UInt16 np = UInt16(sr.readUnsigned(2)); // number of font parameter words

	if (7+nt+lh+(ec-bc+1)+nw+nh+nd+ni+nl+nk+ng+np != lf)
		throw FontMetricException("inconsistent length values");

	setCharRange(bc, ec);
	readHeader(sr);
	is.seekg(28+lh*4);
	readTables(sr, nt, nw, nh, nd, ni);
}


void JFM::readTables (StreamReader &sr, int nt, int nw, int nh, int nd, int ni) {
	// determine smallest charcode with chartype > 0
	UInt16 minchar=0xFFFF, maxchar=0;
	for (int i=0; i < nt; i++) {
		UInt16 c = (UInt16)sr.readUnsigned(2);
		UInt16 t = (UInt16)sr.readUnsigned(2);
		if (t > 0) {
			minchar = min(minchar, c);
			maxchar = max(maxchar, c);
		}
	}
	// build charcode to chartype map
	if (minchar <= maxchar) {
		_minchar = minchar;
		_charTypeTable.resize(maxchar-minchar+1);
		memset(&_charTypeTable[0], 0, nt*sizeof(UInt16));
		sr.seek(-nt*4, ios::cur);
		for (int i=0; i < nt; i++) {
			UInt16 c = (UInt16)sr.readUnsigned(2);
			UInt16 t = (UInt16)sr.readUnsigned(2);
			if (c >= minchar)
				_charTypeTable[c-minchar] = t;
		}
	}
	TFM::readTables(sr, nw, nh, nd, ni);
}


int JFM::charIndex (int c) const {
	UInt16 chartype = 0;
	if (!_charTypeTable.empty() && c >= _minchar && size_t(c) < _minchar+_charTypeTable.size())
		chartype = _charTypeTable[c-_minchar];
	return TFM::charIndex(chartype);
}

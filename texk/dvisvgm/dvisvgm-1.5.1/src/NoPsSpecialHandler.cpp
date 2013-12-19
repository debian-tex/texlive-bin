/*************************************************************************
** NoPsSpecialHandler.cpp                                               **
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
#include "Message.h"
#include "NoPsSpecialHandler.h"

using namespace std;


bool NoPsSpecialHandler::process (const char *prefix, istream &is, SpecialActions *actions) {
	_count++;
	return true;
}


void NoPsSpecialHandler::dviEndPage (unsigned pageno) {
	if (_count > 0) {
		string suffix = (_count > 1 ? "s" : "");
		Message::wstream(true) << _count << " PostScript special" << suffix << " ignored. The resulting SVG might look wrong.\n";
		_count = 0;
	}
}


const char** NoPsSpecialHandler::prefixes () const {
	static const char *pfx[] = {"header=", "psfile=", "PSfile=", "ps:", "ps::", "!", "\"", 0};
	return pfx;
}

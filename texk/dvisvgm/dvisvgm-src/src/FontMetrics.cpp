/*************************************************************************
** FontMetrics.cpp                                                      **
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

#include <config.h>
#include <fstream>
#include "FileFinder.h"
#include "FontMetrics.h"
#include "JFM.h"
#include "TFM.h"

using namespace std;


FontMetrics* FontMetrics::read (const char *fontname) {
	const char *path = FileFinder::lookup(string(fontname) + ".tfm");
	ifstream ifs(path, ios::binary);
	if (!ifs)
		return 0;
	UInt16 id = 256*ifs.get();
	id += ifs.get();
	if (id == 9 || id == 11)  // Japanese font metric file?
		return new JFM(ifs);
	return new TFM(ifs);
}

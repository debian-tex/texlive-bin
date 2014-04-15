/*************************************************************************
** PSPreviewFilter.cpp                                                  **
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
#include <vector>
#include "InputBuffer.h"
#include "InputReader.h"
#include "PSInterpreter.h"
#include "PSPreviewFilter.h"
#include "SpecialActions.h"


using namespace std;

PSPreviewFilter::PSPreviewFilter (PSInterpreter &psi)
	: PSFilter(psi), _active(false), _tightpage(false), _dvi2bp(1.0/65536.0)
{
}


/** Activates this filter so that the PS code will be redirected through it if
 *  it's hooked into the PSInterpreter. */
void PSPreviewFilter::activate () {
	if (_tightpage)  // reactivate filter?
		_active = true;
	else {           // first activation?
		_tightpage = _active = false;
		// try to retrieve version string of preview package set in the PS header section
		if (psInterpreter().executeRaw("SDict begin currentdict/preview@version known{preview@version}{0}ifelse end", 1))
			_version = psInterpreter().rawData()[0];
		// check if tightpage option was set
		if (_version != "0" && psInterpreter().executeRaw("SDict begin preview@tightpage end", 1)) {
			_tightpage = (psInterpreter().rawData()[0] == "true");
			_active = true;
		}
	}
	_boxExtents.clear();
}


/** Tries to extract the bounding box information from a chunk of PostScript code.
 *  @param[in] code pointer to buffer with PS code to filter
 *  @param[in] len number of bytes in buffer */
void PSPreviewFilter::execute (const char *code, size_t len) {
	// If the "tightpage" option was set in the TeX file, 7 integers representing the
	// extent of the bounding box are present at the begin of each page.
	if (!_tightpage)
		psInterpreter().execute(code, len);
	else {
		CharInputBuffer ib(code, len);
		BufferInputReader ir(ib);
		ir.skipSpace();
		int val;
		while (ir.parseInt(val) && _boxExtents.size() <= 7) {
			_boxExtents.push_back(val);
			ir.skipSpace();
		}
	}
	_active = false; // no further processing required
}


/** Returns the bounding box defined by the preview package. */
bool PSPreviewFilter::getBoundingBox (BoundingBox &bbox) const {
	if (_boxExtents.size() < 7)
		return false;
	double left = -_boxExtents[0]*_dvi2bp;
	bbox = BoundingBox(-left, -height(), width()-left, depth());
	return true;
}


/** Gets the 4 border values set by the preview package.
 *  @return true if the border data is available */
bool PSPreviewFilter::getBorders (double &left, double &right, double &top, double &bottom) const {
	if (_boxExtents.size() < 4)
		return false;
	left   = -_boxExtents[0]*_dvi2bp;
	top    = -_boxExtents[1]*_dvi2bp;
	right  = _boxExtents[2]*_dvi2bp;
	bottom = _boxExtents[3]*_dvi2bp;
	return true;
}


/** Returns the box height in PS points, or -1 if no data was found or read yet. */
double PSPreviewFilter::height () const {
	return _boxExtents.size() > 4 ? (_boxExtents[4]-_boxExtents[1])*_dvi2bp : -1;
}


/** Returns the box depth in PS points, or -1 if no data was found or read yet. */
double PSPreviewFilter::depth () const {
	return _boxExtents.size() > 5 ? (_boxExtents[5]+_boxExtents[3])*_dvi2bp : -1;
}


/** Returns the box width in PS points, or -1 if no data was found or read yet. */
double PSPreviewFilter::width () const {
	return _boxExtents.size() > 6 ? (_boxExtents[6]+_boxExtents[2]-_boxExtents[0])*_dvi2bp : -1;
}

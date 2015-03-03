/*************************************************************************
** BoundingBox.cpp                                                      **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2015 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <sstream>
#include <string>
#include "BoundingBox.h"
#include "Matrix.h"
#include "XMLNode.h"
#include "XMLString.h"

using namespace std;


BoundingBox::BoundingBox ()
	: _ulx(0), _uly(0), _lrx(0), _lry(0), _valid(false), _locked(false)
{
}


BoundingBox::BoundingBox (double ulxx, double ulyy, double lrxx, double lryy)
	: _ulx(min(ulxx,lrxx)), _uly(min(ulyy,lryy)),
	  _lrx(max(ulxx,lrxx)), _lry(max(ulyy,lryy)),
	  _valid(true), _locked(false)
{
}


BoundingBox::BoundingBox (const DPair &p1, const DPair &p2)
	: _ulx(min(p1.x(), p2.x())), _uly(min(p1.y(), p2.y())),
	  _lrx(max(p1.x(), p2.x())), _lry(max(p1.y(), p2.y())),
	  _valid(true), _locked(false)
{
}


BoundingBox::BoundingBox (const Length &ulxx, const Length &ulyy, const Length &lrxx, const Length &lryy)
	: _ulx(min(ulxx.pt(),lrxx.pt())), _uly(min(ulyy.pt(),lryy.pt())),
	  _lrx(max(ulxx.pt(),lrxx.pt())), _lry(max(ulyy.pt(),lryy.pt())),
	  _valid(true), _locked(false)
{
}


BoundingBox::BoundingBox (const string &boxstr)
	: _ulx(0), _uly(0), _lrx(0), _lry(0), _valid(false), _locked(false)
{
	set(boxstr);
}


/** Removes leading and trailing whitespace from the given string. */
static string& strip (string &str) {
	size_t n=0;
	while (n < str.length() && isspace(str[n]))
		++n;
	str.erase(0, n);
	n=str.length()-1;
	while (n > 0 && isspace(str[n]))
		--n;
	str.erase(n+1);
	return str;
}


/** Sets or modifies the bounding box. If 'boxstr' consists of 4 length values,
 *  they denote the absolute position of two diagonal corners of the box. In case
 *  of a single length value l the current box is enlarged by adding (-l,-l) the upper
 *  left and (l,l) to the lower right corner.
 *  @param[in] boxstr whitespace and/or comma separated string of lengths. */
void BoundingBox::set (string boxstr) {
	vector<Length> coord;
	const size_t len = boxstr.length();
	size_t l=0;
	strip(boxstr);
	string lenstr;
	do {
		while (l < len && isspace(boxstr[l]))
			l++;
		size_t r=l;
		while (r < len && !isspace(boxstr[r]) && boxstr[r] != ',')
			r++;
		lenstr = boxstr.substr(l, r-l);
		if (!lenstr.empty()) {
			coord.push_back(Length(lenstr));
			if (boxstr[r] == ',')
				r++;
			l = r;
		}
	} while (!lenstr.empty() && coord.size() < 4);

	switch (coord.size()) {
		case 1:
			_ulx -= coord[0].pt();
			_uly -= coord[0].pt();
			_lrx += coord[0].pt();
			_lry += coord[0].pt();
			break;
		case 2:
			_ulx -= coord[0].pt();
			_uly -= coord[1].pt();
			_lrx += coord[0].pt();
			_lry += coord[1].pt();
			break;
		case 4:
			_ulx = min(coord[0].pt(), coord[2].pt());
			_uly = min(coord[1].pt(), coord[3].pt());
			_lrx = max(coord[0].pt(), coord[2].pt());
			_lry = max(coord[1].pt(), coord[3].pt());
			break;
		default:
			throw BoundingBoxException("1, 2 or 4 length parameters expected");
	}
	_valid = true;
}


/** Enlarges the box so that point (x,y) is enclosed. */
void BoundingBox::embed (double x, double y) {
	if (!_locked) {
		if (_valid) {
			if (x < _ulx)
				_ulx = x;
			else if (x > _lrx)
				_lrx = x;
			if (y < _uly)
				_uly = y;
			else if (y > _lry)
				_lry = y;
		}
		else {
			_ulx = _lrx = x;
			_uly = _lry = y;
			_valid = true;
		}
	}
}


/** Enlarges the box so that box bb is enclosed. */
void BoundingBox::embed (const BoundingBox &bb) {
	if (!_locked && bb._valid) {
		if (_valid) {
			embed(bb._ulx, bb._uly);
			embed(bb._lrx, bb._lry);
		}
		else {
			_ulx = bb._ulx;
			_uly = bb._uly;
			_lrx = bb._lrx;
			_lry = bb._lry;
			_valid = true;
		}
	}
}


void BoundingBox::embed (const DPair &c, double r) {
	embed(BoundingBox(c.x()-r, c.y()-r, c.x()+r, c.y()+r));
}


void BoundingBox::expand (double m) {
	if (!_locked) {
		_ulx -= m;
		_uly -= m;
		_lrx += m;
		_lry += m;
	}
}


/** Intersects the current box with bbox and applies the result to *this.
 *  If both boxes are disjoint, *this is not altered.
 *  @param[in] bbox box to intersect with
 *  @return false if *this is locked or both boxes are disjoint */
bool BoundingBox::intersect (const BoundingBox &bbox) {
	if (_locked || _lrx < bbox._ulx || _lry < bbox._uly || _ulx > bbox._lrx || _uly > bbox._lry)
		return false;
	_ulx = max(_ulx, bbox._ulx);
	_uly = max(_uly, bbox._uly);
	_lrx = min(_lrx, bbox._lrx);
	_lry = min(_lry, bbox._lry);
	return true;
}


void BoundingBox::operator += (const BoundingBox &bb) {
	if (!_locked) {
		_ulx += bb._ulx;
		_uly += bb._uly;
		_lrx += bb._lrx;
		_lry += bb._lry;
	}
}


void BoundingBox::scale (double sx, double sy) {
	if (!_locked) {
		_ulx *= sx;
		_lrx *= sx;
		if (sx < 0)	swap(_ulx, _lrx);
		_uly *= sy;
		_lry *= sy;
		if (sy < 0)	swap(_uly, _lry);
	}
}


void BoundingBox::transform (const Matrix &tm) {
	if (!_locked) {
		DPair ul = tm * DPair(_lrx, _lry);
		DPair lr = tm * DPair(_ulx, _uly);
		DPair ll = tm * DPair(_ulx, _lry);
		DPair ur = tm * DPair(_lrx, _uly);
		_ulx = min(min(ul.x(), lr.x()), min(ur.x(), ll.x()));
		_uly = min(min(ul.y(), lr.y()), min(ur.y(), ll.y()));
		_lrx = max(max(ul.x(), lr.x()), max(ur.x(), ll.x()));
		_lry = max(max(ul.y(), lr.y()), max(ur.y(), ll.y()));
	}
}


string BoundingBox::toSVGViewBox () const {
	ostringstream oss;
	oss << XMLString(_ulx) << ' ' << XMLString(_uly) << ' ' << XMLString(width()) << ' ' << XMLString(height());
	return oss.str();
}


ostream& BoundingBox::write (ostream &os) const {
	return os << '('  << _ulx << ", " << _uly
				 << ", " << _lrx << ", " << _lry << ')';
}


XMLElementNode* BoundingBox::toSVGRect () const {
	XMLElementNode *rect = new XMLElementNode("rect");
	rect->addAttribute("x", minX());
	rect->addAttribute("y", minY());
	rect->addAttribute("width", width());
	rect->addAttribute("height", height());
	rect->addAttribute("fill", "none");
	return rect;
}

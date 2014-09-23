/*************************************************************************
** PSPreviewFilter.h                                                    **
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

#ifndef DVISVGM_PSPREVIEWFILTER_H
#define DVISVGM_PSPREVIEWFILTER_H

#include <string>
#include <vector>
#include "BoundingBox.h"
#include "PSFilter.h"

struct SpecialActions;

class PSPreviewFilter : public PSFilter
{
	public:
		PSPreviewFilter (PSInterpreter &psi);
		void activate ();
		void execute (const char *code, size_t len);
		bool active () const                   {return _active;}
		std::string version () const           {return _version;}
		bool tightpage () const                {return _tightpage;}
		void setDviScaleFactor (double dvi2bp) {_dvi2bp = dvi2bp;}
		bool getBorders (double &left, double &right, double &top, double &bottom) const;
		void assignBorders (BoundingBox &bbox) const;
		bool getBoundingBox (BoundingBox &bbox) const;
		double height () const;
		double depth () const;
		double width () const;

	private:
		std::string _version;  ///< version string of preview package
		bool _active;          ///< true if filter is active
		bool _tightpage;       ///< true if tightpage option was given
		double _dvi2bp;        ///< factor to convert dvi units to PS points
		std::vector<int> _boxExtents;
};

#endif

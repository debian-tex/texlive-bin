/*************************************************************************
** EmSpecialHandler.h                                                   **
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

#ifndef EMSPECIALHANDLER_H
#define EMSPECIALHANDLER_H

#include <list>
#include <map>
#include "Pair.h"
#include "SpecialHandler.h"


class EmSpecialHandler : public SpecialHandler, public DVIEndPageListener
{
	struct Line {
		Line (int pp1, int pp2, char cc1, char cc2, double w) : p1(pp1), p2(pp2), c1(cc1), c2(cc2), width(w) {}
		int p1, p2;   ///< point numbers of line ends
		char c1, c2;  ///< cut type of line ends (h, v or p)
		double width; ///< line width
	};

   public:
      EmSpecialHandler ();
		const char* name () const   {return "em";}
		const char* info () const   {return "line drawing statements of the emTeX special set";}
		const char** prefixes () const;
		bool process (const char *prefix, std::istream &in, SpecialActions *actions);

	protected:
		void dviEndPage (unsigned pageno);

   private:
		std::map<int, DPair> _points; ///< points defined by special em:point
		std::list<Line> _lines;       ///< list of lines with undefined end points
		double _linewidth;            ///< global line width
		DPair _pos;                   ///< current position of "graphic cursor"
		SpecialActions *_actions;
};

#endif

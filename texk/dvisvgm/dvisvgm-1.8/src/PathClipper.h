/*************************************************************************
** PathClipper.h                                                        **
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

#ifndef DVISVGM_PATHCLIPPER_H
#define DVISVGM_PATHCLIPPER_H

#include <clipper.hpp>
#include <string>
#include <vector>
#include "Bezier.h"
#include "GraphicPath.h"
#include "MessageException.h"

struct PathClipperException : public MessageException
{
	PathClipperException (const std::string &msg) : MessageException(msg) {}
};


using ClipperLib::IntPoint;

class PathClipper
{
	public:
		typedef GraphicPath<double> CurvedPath;

	public:
		PathClipper () : _numLines(0) {}
		void intersect (const CurvedPath &p1, const CurvedPath &p2, CurvedPath &result);

	protected:
		void flatten (const CurvedPath &gp, ClipperLib::Paths &polygons);
//		void divide (IntPoint &p1, IntPoint &p2, IntPoint &ip);
		void reconstruct (const ClipperLib::Path &polygon, CurvedPath &path);
		void reconstruct (const ClipperLib::Paths &polygons, CurvedPath &path);
		static void callback (IntPoint &e1bot, IntPoint &e1top, IntPoint &e2bot, IntPoint &e2top, IntPoint &pt);

	private:
		std::vector<Bezier> _curves;
		int _numLines;  ///< negative number of straight line segments in path been processed
};

#endif

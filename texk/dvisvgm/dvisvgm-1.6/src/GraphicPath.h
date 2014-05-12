/*************************************************************************
** GraphicPath.h                                                        **
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

#ifndef DVISVGM_GRAPHICPATH_H
#define DVISVGM_GRAPHICPATH_H

#include <list>
#include <ostream>
#include <vector>
#include "BoundingBox.h"
#include "Matrix.h"
#include "Pair.h"
#include "XMLString.h"


template <typename T>
class GraphicPath
{
	public:
		typedef Pair<T> Point;

		struct Command {
			enum Type {MOVETO, LINETO, CONICTO, CUBICTO, CLOSEPATH};

			Command (Type t) : type(t) {}

			Command (Type t, const Point &p) : type(t) {
				params[0] = p;
			}

			Command (Type t, const Point &p1, const Point &p2) : type(t) {
				params[0] = p1;
				params[1] = p2;
			}

			Command (Type t, const Point &p1, const Point &p2, const Point &p3) : type(t) {
				params[0] = p1;
				params[1] = p2;
				params[2] = p3;
			}

			int numParams () const {
				switch (type) {
					case CLOSEPATH : return 0;
					case MOVETO    :
					case LINETO    : return 1;
					case CONICTO   : return 2;
					case CUBICTO   : return 3;
				}
				return 0;
			}

			void transform (const Matrix &matrix) {
				for (int i=0; i < numParams(); i++)
					params[i] = matrix * params[i];
			}

			Type type;
			Point params[3];
		};

		struct Actions
		{
			virtual ~Actions () {}
			virtual void moveto (const Point &p) {}
			virtual void lineto (const Point &p) {}
			virtual void hlineto (const T &y) {}
			virtual void vlineto (const T &x) {}
			virtual void sconicto (const Point &p) {}
			virtual void conicto (const Point &p1, const Point &p2) {}
			virtual void scubicto (const Point &p1, const Point &p2) {}
			virtual void cubicto (const Point &p1, const Point &p2, const Point &p3) {}
			virtual void closepath () {}
			virtual void draw (char cmd, const Point *points, int n) {}
			virtual bool quit () {return false;}
		};

		typedef typename std::vector<Command>::iterator Iterator;
		typedef typename std::vector<Command>::const_iterator ConstIterator;
		typedef typename std::vector<Command>::const_reverse_iterator ConstRevIterator;

   public:
		void newpath () {
			_commands.clear();
		}

		/// Returns true if path is empty (there is nothing to draw)
		bool empty () const {
			return _commands.empty();
		}


		/// Returns the number of path commands used to describe the path.
		size_t size () const {
			return _commands.size();
		}

		void moveto (const T &x, const T &y) {
			moveto(Point(x, y));
		}

		void moveto (const Point &p) {
			// avoid sequences of several MOVETOs; always use latest
			if (_commands.empty() || _commands.back().type != Command::MOVETO)
				_commands.push_back(Command(Command::MOVETO, p));
			else
				_commands.back().params[0] = p;
		}

		void lineto (const T &x, const T &y) {
			lineto(Point(x, y));
		}

		void lineto (const Point &p) {
			_commands.push_back(Command(Command::LINETO, p));
		}

		void conicto (const T &x1, const T &y1, const T &x2, const T &y2) {
			conicto(Point(x1, y1), Point(x2, y2));
		}

		void conicto (const Point &p1, const Point &p2) {
			_commands.push_back(Command(Command::CONICTO, p1, p2));
		}

		void cubicto (const T &x1, const T &y1, const T &x2, const T &y2, const T &x3, const T &y3) {
			cubicto(Point(x1, y1), Point(x2, y2), Point(x3, y3));
		}

		void cubicto (const Point &p1, const Point &p2, const Point &p3) {
			_commands.push_back(Command(Command::CUBICTO, p1, p2, p3));
		}

		void closepath () {
			_commands.push_back(Command(Command::CLOSEPATH));
		}


		const std::vector<Command>& commands () const {
			return _commands;
		}


		/** Detects all open subpaths and closes them by adding a closePath command.
		 *	 Most font formats only support closed outline paths so there are no explicit closePath statements
		 *	 in the glyph's outline description. All open paths are automatically closed by the renderer.
		 *	 This method detects all open paths and adds the missing closePath statement. */
		void closeOpenSubPaths () {
			Command *prevCommand = 0;
			FORALL(_commands, Iterator, it) {
				if (it->type == Command::MOVETO && prevCommand && prevCommand->type != Command::CLOSEPATH) {
					prevCommand = &(*it);
					it = _commands.insert(it, Command(Command::CLOSEPATH))+1;
//					++it; // skip inserted closePath command in next iteration step
				}
				else
					prevCommand = &(*it);
			}
			if (!_commands.empty() && _commands.back().type != Command::CLOSEPATH)
				closepath();
		}


		/** Writes the path data as SVG path drawing command to a given output stream.
		 *  @param[in] os output stream used to write the SVG commands to
		 *  @param[in] sx horizontal scale factor
		 *  @param[in] sy vertical scale factor
		 *  @param[in] dx horizontal translation in PS point units
		 *  @param[in] dy vertical translation in PS point units */
		void writeSVG (std::ostream &os, double sx=1.0, double sy=1.0, double dx=0.0, double dy=0.0) const {
			struct WriteActions : Actions {
				WriteActions (std::ostream &os, double sx, double sy, double dx, double dy)
					: _os(os), _sx(sx), _sy(sy), _dx(dx), _dy(dy) {}

				void draw (char cmd, const Point *points, int n) {
					_os << cmd;
					switch (cmd) {
						case 'H': _os << XMLString(_sx*points->x()+_dx); break;
						case 'V': _os << XMLString(_sy*points->y()+_dy); break;
						default :
							for (int i=0; i < n; i++) {
								if (i > 0)
									_os << ' ';
								_os << XMLString(_sx*points[i].x()+_dx) << ' ' << XMLString(_sy*points[i].y()+_dy);
							}
					}
				}
				std::ostream &_os;
				double _sx, _sy, _dx, _dy;
			} actions(os, sx, sy, dx, dy);
			iterate(actions, true);
		}


		/** Computes the bounding box of the current path.
		 *  @param[out] bbox the computed bounding box */
		void computeBBox (BoundingBox &bbox) const {
			struct BBoxActions : Actions {
				BBoxActions (BoundingBox &bb) : bbox(bb) {}
				void moveto (const Point &p) {bbox.embed(p);}
				void lineto (const Point &p) {bbox.embed(p);}
				void conicto (const Point &p1, const Point &p2) {bbox.embed(p1); bbox.embed(p2);}
				void cubicto (const Point &p1, const Point &p2, const Point &p3) {bbox.embed(p1); bbox.embed(p2); bbox.embed(p3);}
				BoundingBox &bbox;
			} actions(bbox);
			iterate(actions, false);
		}


		/** Checks whether the current path describes a dot/point only (with no extent).
		 *  @param[out] p coordinates of the point if path describes a dot
		 *  @return true if path is a dot/point */
		bool isDot (Point &p) const {
			struct DotActions : Actions {
				DotActions () : differs(false) {}
				void moveto (const Point &p) {point = p;}
				void lineto (const Point &p) {differs = (p != point);}
				void conicto (const Point &p1, const Point &p2) {differs = (point != p1 || point != p2);}
				void cubicto (const Point &p1, const Point &p2, const Point &p3) {differs = (point != p1 || point != p2 || point != p3);}
				bool quit () {return differs;}
				Point point;
				bool differs;
			} actions;
			iterate(actions, false);
			p = actions.point;
			return !actions.differs;
		}


		/** Transforms the path according to a given Matrix.
		 *  @param[in] matrix Matrix describing the affine transformation */
		void transform (const Matrix &matrix) {
			FORALL(_commands, Iterator, it)
				it->transform(matrix);
		}

		void iterate (Actions &actions, bool optimize) const;

   private:
		std::vector<Command> _commands;
};


/** Iterates over all commands defining this path and calls the corresponding template methods.
 *  In the case of successive bezier curve sequences, control points or tangent slopes are often
 *  identical so that the path description contains redundant information. SVG provides shorthand
 *  curve commands that require less parameters. If 'optimize' is true, this method detects such
 *  command sequences.
 *  @param[in] actions template methods called by each iteration step
 *  @param[in] optimize if true, shorthand drawing commands (sconicto, scubicto,...) are considered */
template <typename T>
void GraphicPath<T>::iterate (Actions &actions, bool optimize) const {
	ConstIterator prev = _commands.end();  // pointer to preceding command
	Point fp; // first point of current path
	Point cp; // current point
	Point pstore[2];
	for (ConstIterator it=_commands.begin(); it != _commands.end() && !actions.quit(); ++it) {
		const Point *params = it->params;
		switch (it->type) {
			case Command::MOVETO:
				actions.moveto(params[0]);
				actions.draw('M', params, 1);
				fp = params[0];
				break;
			case Command::LINETO:
				if (optimize) {
					if (cp.x() == params[0].x()) {
						actions.vlineto(params[0].y());
						actions.draw('V', params, 1);
					}
					else if (cp.y() == params[0].y()) {
						actions.hlineto(params[0].x());
						actions.draw('H', params, 1);
					}
					else {
						actions.lineto(params[0]);
						actions.draw('L', params, 1);
					}
				}
				else {
					actions.lineto(params[0]);
					actions.draw('L', params, 1);
				}
				break;
			case Command::CONICTO:
				if (optimize && prev != _commands.end() && prev->type == Command::CONICTO && params[0] == pstore[1]*T(2)-pstore[0]) {
					actions.sconicto(params[1]);
					actions.draw('T', params+1, 1);
				}
				else {
					actions.conicto(params[0], params[1]);
					actions.draw('Q', params, 2);
				}
				pstore[0] = params[0]; // store control point and
				pstore[1] = params[1]; // curve endpoint
				break;
			case Command::CUBICTO:
				// is first control point reflection of preceding second control point?
				if (optimize && prev != _commands.end() && prev->type == Command::CUBICTO && params[0] == pstore[1]*T(2)-pstore[0]) {
					actions.scubicto(params[1], params[2]);
					actions.draw('S', params+1, 2);
				}
				else {
					actions.cubicto(params[0], params[1], params[2]);
					actions.draw('C', params, 3);
				}
				pstore[0] = params[1]; // store second control point and
				pstore[1] = params[2]; // curve endpoint
				break;
			case Command::CLOSEPATH:
				actions.closepath();
				actions.draw('Z', params, 0);
				cp = fp;
		}
		// update current point
		const int np = it->numParams();
		if (np > 0)
			cp = it->params[np-1];
		prev = it;
	}
}

#endif

/*************************************************************************
** PsSpecialHandler.hpp                                                 **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2017 Martin Gieseking <martin.gieseking@uos.de>   **
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

#ifndef PSSPECIALHANDLER_HPP
#define PSSPECIALHANDLER_HPP

#include <set>
#include <stack>
#include <string>
#include <vector>
#include "GraphicsPath.hpp"
#include "PSInterpreter.hpp"
#include "SpecialHandler.hpp"
#include "PSPattern.hpp"
#include "PSPreviewFilter.hpp"


class PSPattern;
class XMLElementNode;

class PsSpecialHandler : public SpecialHandler, public DVIEndPageListener, protected PSActions
{
	typedef GraphicsPath<double> Path;
	typedef std::vector<double>::const_iterator DoubleVecIt;
	typedef Color::ColorSpace ColorSpace;

	class ClippingStack
	{
		public:
			void pushEmptyPath ();
			void push (const Path &path, int saveID=-1);
			void replace (const Path &path);
			void dup (int saveID=-1);
			void pop (int saveID=-1, bool grestore=false);
			void clear ();
			bool empty () const {return _stack.empty();}
			bool clippathLoaded () const;
			void setClippathLoaded (bool loaded);
			const Path* top () const;
			Path* getPath (size_t id);
			int topID () const {return _stack.empty() ? 0 : _stack.top().pathID;}

		private:
			struct Entry {
				int pathID;        ///< index referencing a path of the pool
				int saveID;        ///< if >=0, path was pushed by 'save', and saveID holds the ID of the
				bool cpathLoaded;  ///< true if clipping path was loaded into current path
				Entry (int pid, int sid) : pathID(pid), saveID(sid), cpathLoaded(false) {}
			};
			std::vector<Path> _paths;  ///< pool of all clipping paths
			std::stack<Entry> _stack;
	};

	enum PsSection {PS_NONE, PS_HEADERS, PS_BODY};

	public:
		PsSpecialHandler ();
		~PsSpecialHandler ();
		const char* name () const override {return "ps";}
		const char* info () const override {return "dvips PostScript specials";}
		const char** prefixes () const override;
		void preprocess (const char *prefix, std::istream &is, SpecialActions &actions) override;
		bool process (const char *prefix, std::istream &is, SpecialActions &actions) override;
		void setDviScaleFactor (double dvi2bp) override {_previewFilter.setDviScaleFactor(dvi2bp);}
		void enterBodySection ();

	public:
		static bool COMPUTE_CLIPPATHS_INTERSECTIONS;
		static bool SHADING_SEGMENT_OVERLAP;
		static int SHADING_SEGMENT_SIZE;
		static double SHADING_SIMPLIFY_DELTA;

	protected:
		void initialize ();
		void initgraphics ();
		void moveToDVIPos ();
		void executeAndSync (std::istream &is, bool updatePos);
		void processHeaderFile (const char *fname);
		void psfile (const std::string &fname, const std::map<std::string,std::string> &attr);
		void dviEndPage (unsigned pageno, SpecialActions &actions) override;
		void clip (Path &path, bool evenodd);
		void processSequentialPatchMesh (int shadingTypeID, ColorSpace cspace, VectorIterator<double> &it);
		void processLatticeTriangularPatchMesh (ColorSpace colorSpace, VectorIterator<double> &it);

		/// scale given value by current PS scale factors
		double scale (double v) const {return v*(_sx*_cos*_cos + _sy*(1-_cos*_cos));}

		void applyscalevals (std::vector<double> &p) override  {_sx = p[0]; _sy = p[1]; _cos = p[2];}
		void clip (std::vector<double> &p) override            {clip(p, false);}
		void clip (std::vector<double> &p, bool evenodd);
		void clippath (std::vector<double> &p) override;
		void closepath (std::vector<double> &p) override;
		void curveto (std::vector<double> &p) override;
		void eoclip (std::vector<double> &p) override          {clip(p, true);}
		void eofill (std::vector<double> &p) override          {fill(p, true);}
		void fill (std::vector<double> &p, bool evenodd);
		void fill (std::vector<double> &p) override            {fill(p, false);}
		void grestore (std::vector<double> &p) override;
		void grestoreall (std::vector<double> &p) override;
		void gsave (std::vector<double> &p) override;
		void initclip (std::vector<double> &p) override;
		void lineto (std::vector<double> &p) override;
		void makepattern (std::vector<double> &p) override;
		void moveto (std::vector<double> &p) override;
		void newpath (std::vector<double> &p) override;
		void querypos (std::vector<double> &p) override        {_currentpoint = DPair(p[0], p[1]);}
		void restore (std::vector<double> &p) override;
		void rotate (std::vector<double> &p) override;
		void save (std::vector<double> &p) override;
		void scale (std::vector<double> &p) override;
		void setcmykcolor (std::vector<double> &cmyk) override;
		void setdash (std::vector<double> &p) override;
		void setgray (std::vector<double> &p) override;
		void sethsbcolor (std::vector<double> &hsb) override;
		void setlinecap (std::vector<double> &p) override      {_linecap = uint8_t(p[0]);}
		void setlinejoin (std::vector<double> &p) override     {_linejoin = uint8_t(p[0]);}
		void setlinewidth (std::vector<double> &p) override    {_linewidth = p[0] ? scale(p[0])*1.00375 : 0.5;}
		void setmatrix (std::vector<double> &p) override;
		void setmiterlimit (std::vector<double> &p) override   {_miterlimit = p[0]*1.00375;}
		void setopacityalpha (std::vector<double> &p) override {_opacityalpha = p[0];}
		void setpattern (std::vector<double> &p) override;
		void setrgbcolor (std::vector<double> &rgb) override;
		void shfill (std::vector<double> &p) override;
		void stroke (std::vector<double> &p) override;
		void translate (std::vector<double> &p) override;
		void executed () override;

	private:
		PSInterpreter _psi;
		SpecialActions *_actions;
		PSPreviewFilter _previewFilter;  ///< filter to extract information generated by the preview package
		PsSection _psSection;       ///< current section processed (nothing yet, headers, or body specials)
		XMLElementNode *_xmlnode;   ///< if != 0, created SVG elements are appended to this node
		XMLElementNode *_savenode;  ///< pointer to temporaryly store _xmlnode
		std::string _headerCode;    ///< collected literal PS header code
		Path _path;
		DPair _currentpoint;        ///< current PS position in bp units
		Color _currentcolor;        ///< current stroke/fill color
		double _sx, _sy;            ///< horizontal and vertical scale factors retrieved by operator "applyscalevals"
		double _cos;                ///< cosine of angle between (1,0) and transform(1,0)
		double _linewidth;          ///< current linewidth
		double _miterlimit;         ///< current miter limit
		double _opacityalpha;       ///< opacity level (0=fully transparent, ..., 1=opaque)
		uint8_t _linecap  : 2;      ///< current line cap (0=butt, 1=round, 2=projecting square)
		uint8_t _linejoin : 2;      ///< current line join (0=miter, 1=round, 2=bevel)
		double _dashoffset;         ///< current dash offset
		std::vector<double> _dashpattern;
		ClippingStack _clipStack;
		std::map<int, PSPattern*> _patterns;
		PSTilingPattern *_pattern;  ///< current pattern
};

#endif

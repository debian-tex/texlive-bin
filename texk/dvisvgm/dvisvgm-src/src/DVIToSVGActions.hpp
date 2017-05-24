/*************************************************************************
** DVIToSVGActions.hpp                                                  **
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

#ifndef DVITOSVGACTIONS_HPP
#define DVITOSVGACTIONS_HPP

#include <map>
#include <set>
#include "BoundingBox.hpp"
#include "DVIActions.hpp"
#include "Matrix.hpp"
#include "SpecialActions.hpp"
#include "SpecialManager.hpp"
#include "SVGTree.hpp"


class DVIToSVG;
class FileFinder;
class Font;
class XMLNode;

class DVIToSVGActions : public DVIActions, public SpecialActions
{
	typedef std::map<const Font*, std::set<int>> CharMap;
	typedef std::set<const Font*> FontSet;
	typedef std::map<std::string,BoundingBox> BoxMap;

	public:
		DVIToSVGActions (DVIToSVG &dvisvg, SVGTree &svg);
		void reset ();
		void setChar (double x, double y, unsigned c, bool vertical, const Font &f) override;
		void setRule (double x, double y, double height, double width) override;
		void setBgColor (const Color &color) override;
		void setColor (const Color &color) override             {_svg.setColor(color);}
		void setMatrix (const Matrix &m) override               {_svg.setMatrix(m);}
		const Matrix& getMatrix () const override               {return _svg.getMatrix();}
		void getPageTransform (Matrix &matrix) const override   {_dvireader->getPageTransformation(matrix);}
		Color getColor () const override                        {return _svg.getColor();}
		int getDVIStackDepth() const override                   {return _dvireader->stackDepth();}
		unsigned getCurrentPageNumber() const override          {return _dvireader->currentPageNumber();}
		void appendToPage (XMLNode *node) override              {_svg.appendToPage(node);}
		void appendToDefs (XMLNode *node) override              {_svg.appendToDefs(node);}
		void prependToPage (XMLNode *node) override             {_svg.prependToPage(node);}
		void pushContextElement (XMLElementNode *node) override {_svg.pushContextElement(node);}
		void popContextElement () override                      {_svg.popContextElement();}
		void setTextOrientation(bool vertical) override         {_svg.setVertical(vertical);}
		void moveToX (double x) override;
		void moveToY (double y) override;
		void setFont (int num, const Font &font) override;
		void special (const std::string &spc, double dvi2bp, bool preprocessing=false) override;
		void beginPage (unsigned pageno, const std::vector<int32_t> &c) override;
		void endPage (unsigned pageno) override;
		void progress (size_t current, size_t total, const char *id=0) override;
		void progress (const char *id) override;
		double getX() const override  {return _dvireader->getXPos();}
		double getY() const override  {return _dvireader->getYPos();}
		void setX (double x) override {_dvireader->translateToX(x); _svg.setX(x);}
		void setY (double y) override {_dvireader->translateToY(y); _svg.setY(y);}
		void finishLine () override   {_dvireader->finishLine();}
		BoundingBox& bbox () override {return _bbox;}
		BoundingBox& bbox (const std::string &name, bool reset=false) override;
		void embed (const BoundingBox &bbox) override;
		void embed (const DPair &p, double r=0) override;
		std::string getSVGFilename (unsigned pageno) const override;
		std::string getBBoxFormatString () const override;
		CharMap& getUsedChars () const        {return _usedChars;}
		const FontSet& getUsedFonts () const  {return _usedFonts;}
		void setDVIReader (BasicDVIReader &r) {_dvireader = &r;}

	private:
		SVGTree &_svg;
		BasicDVIReader *_dvireader;
		BoundingBox _bbox;
		int _pageCount;
		int _currentFontNum;
		mutable CharMap _usedChars;
		FontSet _usedFonts;
		Color _bgcolor;
		BoxMap _boxes;
};


#endif

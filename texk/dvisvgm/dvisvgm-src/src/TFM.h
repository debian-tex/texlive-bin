/*************************************************************************
** TFM.h                                                                **
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

#ifndef DVISVGM_TFM_H
#define DVISVGM_TFM_H

#include <istream>
#include <string>
#include <vector>
#include "FontMetrics.h"
#include "types.h"
#include "StreamReader.h"

class StreamReader;

class TFM : public FontMetrics
{
	public:
//		TFM (const char *fname);
		TFM (std::istream &is);
		double getDesignSize () const;
		double getCharWidth (int c) const;
		double getCharHeight (int c) const;
		double getCharDepth (int c) const;
		double getItalicCorr (int c) const;
		bool verticalLayout () const {return false;}
		UInt32 getChecksum () const  {return _checksum;}
		UInt16 firstChar () const    {return _firstChar;}
		UInt16 lastChar () const     {return _lastChar;}

	protected:
		TFM () : _checksum(0), _firstChar(0), _lastChar(0), _designSize(0) {}
		void readHeader (StreamReader &sr);
		void readTables (StreamReader &sr, int nw, int nh, int nd, int ni);
		virtual int charIndex (int c) const;
		void setCharRange (int firstchar, int lastchar) {_firstChar=firstchar; _lastChar=lastchar;}

	private:
		UInt32 _checksum;
		UInt16 _firstChar, _lastChar;
		FixWord _designSize;  ///< design size of the font in TeX points (7227 pt = 254 cm)
		std::vector<UInt32>  _charInfoTable;
		std::vector<FixWord> _widthTable;    ///< character widths in design size units
		std::vector<FixWord> _heightTable;   ///< character height in design size units
		std::vector<FixWord> _depthTable;    ///< character depth in design size units
		std::vector<FixWord> _italicTable;   ///< italic corrections in design size units
};

#endif

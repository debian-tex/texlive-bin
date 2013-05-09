/*************************************************************************
** VFReader.h                                                           **
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

#ifndef VFREADER_H
#define VFREADER_H

#include "MessageException.h"
#include "StreamReader.h"
#include "types.h"


struct VFException : public MessageException
{
	VFException (const std::string &msg) : MessageException(msg) {}
};


struct VFActions;


class VFReader : public StreamReader
{
	typedef bool (*ApproveAction)(int);
	public:
		VFReader (std::istream &is);
		virtual ~VFReader ();
		VFActions* replaceActions (VFActions *a);
		bool executeAll ();
		bool executePreambleAndFontDefs ();
		bool executeCharDefs ();

	protected:
		int executeCommand (ApproveAction approve=0);

		// the following methods represent the VF commands
		// they are called by executeCommand and should not be used directly
		void cmdPre ();
		void cmdPost ();
		void cmdShortChar (int pl);
		void cmdLongChar ();
		void cmdFontDef (int len);

	private:
		VFActions *actions; ///< actions to execute when reading a VF command
		double    designSize; ///< design size of currently read VF
};

#endif

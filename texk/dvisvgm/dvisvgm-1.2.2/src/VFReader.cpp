/*************************************************************************
** VFReader.cpp                                                         **
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

#include <sstream>
#include "Font.h"
#include "VFActions.h"
#include "VFReader.h"
#include "macros.h"

using namespace std;


/** Converts a TFM fix point value to double. */
inline static double fix2double (FixWord fix) {
	return double(fix)/(1 << 20);
}


VFReader::VFReader (istream &is)
	: StreamReader(is), _actions(0), _designSize(0) {
}


VFReader::~VFReader () {
}


VFActions* VFReader::replaceActions (VFActions *a) {
	VFActions *ret = _actions;
	_actions = a;
	return ret;
}


/** Reads a single VF command from the current position of the input stream and calls the
 *  corresponding cmdFOO method. The execution can be influenced by a function of type ApproveOpcode.
 *  It takes an opcode and returns true if the command is supposed to be executed.
 *  @param[in] approve function to approve invocation of the action assigned to command
 *  @return opcode of the executed command */
int VFReader::executeCommand (ApproveAction approve) {
	int opcode = in().get();
	if (!in() || opcode < 0)  // at end of file
		throw VFException("invalid file");

	bool approved = !approve || approve(opcode);
	VFActions *actions = _actions;
	if (!approved)
		replaceActions(0);   // disable actions

	if (opcode <= 241)      // short character definition?
		cmdShortChar(opcode);
	else if (opcode >= 243 && opcode <= 246)   // font definition?
		cmdFontDef(opcode-243+1);
	else {
		switch (opcode) {
			case 242: cmdLongChar(); break;      // long character definition
			case 247: cmdPre();      break;      // preamble
			case 248: cmdPost();     break;      // postamble
			default : {                          // invalid opcode
				replaceActions(actions);              // reenable actions
				ostringstream oss;
				oss << "undefined VF command (opcode " << opcode << ')';
				throw VFException(oss.str());
			}
		}
	}
	replaceActions(actions); // reenable actions
	return opcode;
}


bool VFReader::executeAll () {
  	in().clear();   // reset all status bits
	if (!in())
		return false;
	in().seekg(0);  // move file pointer to first byte of the input stream
	while (!in().eof() && executeCommand() != 248); // stop reading after post (248)
	return true;
}


/// Returns true if op indicates the preamble or a font definition
static bool is_pre_or_fontdef (int op) {return op > 242;}
static bool is_chardef (int op)        {return op < 243;}


bool VFReader::executePreambleAndFontDefs () {
	in().clear();
	if (!in())
		return false;
	in().seekg(0);  // move file pointer to first byte of the input stream
	while (!in().eof() && executeCommand(is_pre_or_fontdef) > 242); // stop reading after last font definition
	return true;
}


bool VFReader::executeCharDefs () {
	in().clear();
	if (!in())
		return false;
	in().seekg(0);
	while (!in().eof() && executeCommand(is_chardef) < 243); // stop reading after last char definition
	return true;
}

//////////////////////////////////////////////////////////////////////////////

/** Reads and executes DVI preamble command. */
void VFReader::cmdPre () {
	UInt32 i   = readUnsigned(1);  // identification number (should be 2)
	UInt32 k   = readUnsigned(1);  // length of following comment
	string cmt = readString(k);    // comment
	UInt32 cs  = readUnsigned(4);  // check sum to be compared with TFM cecksum
	UInt32 ds  = readUnsigned(4);  // design size (same as TFM design size) (fix_word)
	_designSize = fix2double(ds);
	if (i != 202)
		throw VFException("invalid identification value in preamble");
   if (_actions)
      _actions->preamble(cmt, cs, ds);
}


void VFReader::cmdPost () {
	while ((readUnsigned(1)) == 248); // skip fill bytes
	if (_actions)
		_actions->postamble();
}


void VFReader::cmdLongChar () {
	UInt32 pl  = readUnsigned(4);      // packet length (length of DVI subroutine)
	if (_actions) {
		UInt32 cc  = readUnsigned(4);   // character code
		readUnsigned(4);                // character width from corresponding TFM file
		vector<UInt8> *dvi = new vector<UInt8>(pl); // DVI subroutine
		readBytes(pl, *dvi);
		_actions->defineVFChar(cc, dvi); // call template method for user actions
	}
	else
		in().seekg(8+pl, ios::cur);     // skip remaining char definition bytes
}


/** Reads and executes short_char_x command.
 *  @param[in] pl packet length (length of DVI subroutine) */
void VFReader::cmdShortChar (int pl) {
	if (_actions) {
		UInt32 cc  = readUnsigned(1);   // character code
		readUnsigned(3);                // character width from corresponding TFM file
		vector<UInt8> *dvi = new vector<UInt8>(pl); // DVI subroutine
		readBytes(pl, *dvi);
		_actions->defineVFChar(cc, dvi); // call template method for user actions
	}
	else
		in().seekg(4+pl, ios::cur);     // skip char definition bytes
}


void VFReader::cmdFontDef (int len) {
	UInt32 fontnum  = readUnsigned(len);   // font number
	UInt32 checksum = readUnsigned(4);     // font checksum (to be compared with corresponding TFM checksum)
	UInt32 ssize    = readUnsigned(4);     // scaled size of font relative to design size (fix_word)
	UInt32 dsize    = readUnsigned(4);     // design size of font (same as TFM design size) (fix_word)
	UInt32 pathlen  = readUnsigned(1);     // length of font path
	UInt32 namelen  = readUnsigned(1);     // length of font name
	string fontpath = readString(pathlen);
	string fontname = readString(namelen);
	if (_actions) {
		double ss = fix2double(ssize);
		double ds = fix2double(dsize);
		_actions->defineVFFont(fontnum, fontpath, fontname, checksum, ds, ss*_designSize);
	}
}

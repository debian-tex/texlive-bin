/*************************************************************************
** BasicDVIReader.cpp                                                   **
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

#include <config.h>
#include <algorithm>
#include <sstream>
#include "BasicDVIReader.h"

using namespace std;


BasicDVIReader::BasicDVIReader (std::istream &is) : StreamReader(is), _dviFormat(DVI_NONE)
{
}


/** Evaluates the next DVI command, and computes the corresponding handler.
 *  @param[out] handler handler for current DVI command
 *  @param[out] param the handler must be called with this parameter
 *  @return opcode of current DVI command */
int BasicDVIReader::evalCommand (CommandHandler &handler, int &param) {
	struct DVICommand {
		CommandHandler handler;
		int length;  // number of parameter bytes
	};

	/* Each cmdFOO command reads the necessary number of bytes from the stream, so executeCommand
	doesn't need to know the exact DVI command format. Some cmdFOO methods are used for multiple
	DVI commands because they only differ in length of their parameters. */
	static const DVICommand commands[] = {
		{&BasicDVIReader::cmdSetChar, 1},     {&BasicDVIReader::cmdSetChar, 2},      // 128-129
		{&BasicDVIReader::cmdSetChar, 3},     {&BasicDVIReader::cmdSetChar, 4},      // 130-131
		{&BasicDVIReader::cmdSetRule, 8},                                            // 132
		{&BasicDVIReader::cmdPutChar, 1},     {&BasicDVIReader::cmdPutChar, 2},      // 133-134
		{&BasicDVIReader::cmdPutChar, 3},     {&BasicDVIReader::cmdPutChar, 4},      // 135-136
		{&BasicDVIReader::cmdPutRule, 8},                                            // 137
		{&BasicDVIReader::cmdNop, 0},                                                // 138
		{&BasicDVIReader::cmdBop, 44},        {&BasicDVIReader::cmdEop, 0},          // 139-140
		{&BasicDVIReader::cmdPush, 0},        {&BasicDVIReader::cmdPop, 0},          // 141-142
		{&BasicDVIReader::cmdRight, 1},       {&BasicDVIReader::cmdRight, 2},        // 143-144
		{&BasicDVIReader::cmdRight, 3},       {&BasicDVIReader::cmdRight, 4},        // 145-146
		{&BasicDVIReader::cmdW0, 0},                                                 // 147
		{&BasicDVIReader::cmdW, 1},           {&BasicDVIReader::cmdW, 2},            // 148-149
		{&BasicDVIReader::cmdW, 3},           {&BasicDVIReader::cmdW, 4},            // 150-151
		{&BasicDVIReader::cmdX0, 0},                                                 // 152
		{&BasicDVIReader::cmdX, 1},           {&BasicDVIReader::cmdX, 2},            // 153-154
		{&BasicDVIReader::cmdX, 3},           {&BasicDVIReader::cmdX, 4},            // 155-156
		{&BasicDVIReader::cmdDown, 1},        {&BasicDVIReader::cmdDown, 2},         // 157-158
		{&BasicDVIReader::cmdDown, 3},        {&BasicDVIReader::cmdDown, 4},         // 159-160
		{&BasicDVIReader::cmdY0, 0},                                                 // 161
		{&BasicDVIReader::cmdY, 1},           {&BasicDVIReader::cmdY, 2},            // 162-163
		{&BasicDVIReader::cmdY, 3},           {&BasicDVIReader::cmdY, 4},            // 164-165
		{&BasicDVIReader::cmdZ0, 0},                                                 // 166
		{&BasicDVIReader::cmdZ, 1},           {&BasicDVIReader::cmdZ, 2},            // 167-168
		{&BasicDVIReader::cmdZ, 3},           {&BasicDVIReader::cmdZ, 4},            // 169-170

		{&BasicDVIReader::cmdFontNum, 1},     {&BasicDVIReader::cmdFontNum, 2},      // 235-236
		{&BasicDVIReader::cmdFontNum, 3},     {&BasicDVIReader::cmdFontNum, 4},      // 237-238
		{&BasicDVIReader::cmdXXX, 1},         {&BasicDVIReader::cmdXXX, 2},          // 239-240
		{&BasicDVIReader::cmdXXX, 3},         {&BasicDVIReader::cmdXXX, 4},          // 241-242
		{&BasicDVIReader::cmdFontDef, 1},     {&BasicDVIReader::cmdFontDef, 2},      // 243-244
		{&BasicDVIReader::cmdFontDef, 3},     {&BasicDVIReader::cmdFontDef, 4},      // 245-246
		{&BasicDVIReader::cmdPre, 0},         {&BasicDVIReader::cmdPost, 0},         // 247-248
		{&BasicDVIReader::cmdPostPost, 0},                                           // 249
	};

	const int opcode = readByte();
	if (!isStreamValid() || opcode < 0)  // at end of file
		throw InvalidDVIFileException("invalid DVI file");

	int num_param_bytes = 0;
	param = -1;
	if (opcode >= 0 && opcode <= 127) {
		handler = &BasicDVIReader::cmdSetChar0;
		param = opcode;
	}
	else if (opcode >= 171 && opcode <= 234) {
		handler = &BasicDVIReader::cmdFontNum0;
		param = opcode-171;
	}
	else if ((_dviFormat == DVI_XDVOLD && opcode >= 251 && opcode <= 254)
			  || (_dviFormat == DVI_XDVNEW && opcode >= 252 && opcode <= 253)) {  // XDV command?
		static const CommandHandler handlers[] = {
			&BasicDVIReader::cmdXPic,
			&BasicDVIReader::cmdXFontDef,
			&BasicDVIReader::cmdXGlyphArray,
			&BasicDVIReader::cmdXGlyphString
		};
		handler = handlers[opcode-251];
		param = 0;
	}
	else if (_dviFormat == DVI_PTEX && opcode == 255) {  // direction command set by pTeX?
		handler = &BasicDVIReader::cmdDir;
		num_param_bytes = 1;
	}
	else if (opcode >= 250) {
		ostringstream oss;
		oss << "undefined DVI command (opcode " << opcode << ')';
		throw DVIException(oss.str());
	}
	else {
		const int offset = opcode <= 170 ? 128 : 235-(170-128+1);
		handler = commands[opcode-offset].handler;
		num_param_bytes = commands[opcode-offset].length;
	}
	if (param < 0)
		param = num_param_bytes;
	return opcode;
}


/** Reads a single DVI command from the current position of the input stream and calls the
 *  corresponding cmdFOO method.
 *  @return opcode of the executed command */
int BasicDVIReader::executeCommand () {
	CommandHandler handler;
	int param; // parameter of handler
	int opcode = evalCommand(handler, param);
	(this->*handler)(param);
	return opcode;
}


void BasicDVIReader::executePostPost () {
	clearStream();  // reset all status bits
	if (!isStreamValid())
		throw DVIException("invalid DVI file");

	seek(-1, ios::end);       // stream pointer to last byte
	int count=0;
	while (peek() == 223) {   // count trailing fill bytes
		seek(-1, ios::cur);
		count++;
	}
	if (count < 4)  // the standard requires at least 4 trailing fill bytes
		throw DVIException("missing fill bytes at end of file");

	setDVIFormat((DVIFormat)readUnsigned(1));
}


void BasicDVIReader::executeAllPages () {
	if (_dviFormat == DVI_NONE)
		executePostPost();             // get version ID from post_post
	seek(0);                          // go to preamble
	while (executeCommand() != 248);  // execute all commands until postamble is reached
}


void BasicDVIReader::setDVIFormat (DVIFormat format) {
	_dviFormat = max(_dviFormat, format);
	switch (_dviFormat) {
		case DVI_STANDARD:
		case DVI_PTEX:
		case DVI_XDVOLD:
		case DVI_XDVNEW:
			break;
		default:
			ostringstream oss;
			oss << "DVI format " << _dviFormat << " not supported";
			throw DVIException(oss.str());
	}
}

/////////////////////////////////////

/** Executes preamble command.
 *  Format: pre i[1] num[4] den[4] mag[4] k[1] x[k] */
void BasicDVIReader::cmdPre (int) {
	setDVIFormat((DVIFormat)readUnsigned(1)); // identification number
	seek(12, ios::cur);         // skip numerator, denominator, and mag factor
	UInt32 k = readUnsigned(1); // length of following comment
	seek(k, ios::cur);          // skip comment
}


/** Executes postamble command.
 *  Format: post p[4] num[4] den[4] mag[4] l[4] u[4] s[2] t[2] */
void BasicDVIReader::cmdPost (int) {
	seek(28, ios::cur);
}


/** Executes postpost command.
 *  Format: postpost q[4] i[1] 223’s[>= 4] */
void BasicDVIReader::cmdPostPost (int) {
	seek(4, ios::cur);
	setDVIFormat((DVIFormat)readUnsigned(1));  // identification byte
	while (readUnsigned(1) == 223);  // skip fill bytes (223), eof bit should be set now
}


/** Executes bop (begin of page) command.
 *  Format: bop c0[+4] ... c9[+4] p[+4] */
void BasicDVIReader::cmdBop (int)         {seek(44, ios::cur);}
void BasicDVIReader::cmdEop (int)         {}
void BasicDVIReader::cmdPush (int)        {}
void BasicDVIReader::cmdPop (int)         {}
void BasicDVIReader::cmdSetChar0 (int)    {}
void BasicDVIReader::cmdSetChar (int len) {seek(len, ios::cur);}
void BasicDVIReader::cmdPutChar (int len) {seek(len, ios::cur);}
void BasicDVIReader::cmdSetRule (int)     {seek(8, ios::cur);}
void BasicDVIReader::cmdPutRule (int)     {seek(8, ios::cur);}
void BasicDVIReader::cmdRight (int len)   {seek(len, ios::cur);}
void BasicDVIReader::cmdDown (int len)    {seek(len, ios::cur);}
void BasicDVIReader::cmdX0 (int)          {}
void BasicDVIReader::cmdY0 (int)          {}
void BasicDVIReader::cmdW0 (int)          {}
void BasicDVIReader::cmdZ0 (int)          {}
void BasicDVIReader::cmdX (int len)       {seek(len, ios::cur);}
void BasicDVIReader::cmdY (int len)       {seek(len, ios::cur);}
void BasicDVIReader::cmdW (int len)       {seek(len, ios::cur);}
void BasicDVIReader::cmdZ (int len)       {seek(len, ios::cur);}
void BasicDVIReader::cmdNop (int)         {}
void BasicDVIReader::cmdDir (int)         {seek(1, ios::cur);}
void BasicDVIReader::cmdFontNum0 (int)    {}
void BasicDVIReader::cmdFontNum (int len) {seek(len, ios::cur);}
void BasicDVIReader::cmdXXX (int len)     {seek(readUnsigned(len), ios::cur);}


/** Executes fontdef command.
 *  Format fontdef k[len] c[4] s[4] d[4] a[1] l[1] n[a+l]
 * @param[in] len size of font number variable (in bytes) */
void BasicDVIReader::cmdFontDef (int len) {
	seek(len+12, ios::cur);             // skip font number
	UInt32 pathlen  = readUnsigned(1);  // length of font path
	UInt32 namelen  = readUnsigned(1);  // length of font name
	seek(pathlen+namelen, ios::cur);
}


/** XDV extension: include image or pdf file.
 *  parameters: box[1] matrix[4][6] p[2] len[2] path[l] */
void BasicDVIReader::cmdXPic (int) {
	seek(1+24+2, ios::cur);
	UInt16 len = readUnsigned(2);
	seek(len, ios::cur);
}


void BasicDVIReader::cmdXFontDef (int) {
	seek(4+4, ios::cur);
	UInt16 flags = readUnsigned(2);
	UInt8 len = readUnsigned(1);
	if (_dviFormat == DVI_XDVOLD)
		len += readUnsigned(1)+readUnsigned(1);
	seek(len, ios::cur);
	if (_dviFormat == DVI_XDVNEW)
		seek(4, ios::cur); // skip subfont index
	if (flags & 0x0200)   // colored?
		seek(4, ios::cur);
	if (flags & 0x1000)   // extend?
		seek(4, ios::cur);
	if (flags & 0x2000)   // slant?
		seek(4, ios::cur);
	if (flags & 0x4000)   // embolden?
		seek(4, ios::cur);
	if ((flags & 0x0800) && (_dviFormat == DVI_XDVOLD)) { // variations?
		UInt16 num_variations = readSigned(2);
		seek(4*num_variations, ios::cur);
	}
}


void BasicDVIReader::cmdXGlyphArray (int) {
	seek(4, ios::cur);
	UInt16 num_glyphs = readUnsigned(2);
	seek(10*num_glyphs, ios::cur);
}


void BasicDVIReader::cmdXGlyphString (int) {
	seek(4, ios::cur);
	UInt16 num_glyphs = readUnsigned(2);
	seek(6*num_glyphs, ios::cur);
}

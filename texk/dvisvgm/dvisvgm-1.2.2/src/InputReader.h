/*************************************************************************
** InputReader.h                                                        **
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

#ifndef INPUTREADER_H
#define INPUTREADER_H

#include <istream>
#include <map>
#include <string>
#include "InputBuffer.h"


class InputReader
{
	public:
		virtual ~InputReader() {}
		virtual int get () =0;
		virtual int peek () const =0;
		virtual int peek (size_t n) const =0;
		virtual bool eof () const =0;
		virtual bool check (char c) const {return peek() == c;}
		virtual bool check (const char *s, bool consume=true);
		virtual int compare (const char *s, bool consume=true);
		virtual void skip (size_t n);
		virtual bool skipUntil (const char *s, bool consume=true);
		virtual int find (char c) const;
		virtual void skipSpace ();
		virtual int getInt ();
		virtual bool parseInt (int &val, bool accept_sign=true);
		virtual bool parseInt (int base, int &val);
		virtual bool parseUInt (unsigned &val);
		virtual char parseDouble (double &val);
		virtual double getDouble ();
		virtual std::string getWord ();
		virtual char getPunct ();
		virtual std::string getQuotedString (char quotechar);
		virtual std::string getString ();
		virtual std::string getString (size_t n);
		virtual int parseAttributes (std::map<std::string,std::string> &attr);
		virtual operator bool () const {return !eof();}
};


class StreamInputReader : public InputReader
{
	public:
		StreamInputReader (std::istream &is) : _is(is) {}
		int get ()                {return _is.get();}
		int peek () const         {return _is.peek();}
		int peek (size_t n) const;
		bool eof () const         {return !_is || _is.eof();}

	private:
		std::istream &_is;
};


class BufferInputReader : public InputReader
{
	public:
		BufferInputReader (InputBuffer &ib) : _ib(&ib) {}
		void assign (InputBuffer &ib) {_ib = &ib;}
		int get ()                    {return _ib->get();}
		int peek () const             {return _ib->peek();}
		int peek (size_t n) const     {return _ib->peek(n);}
		bool eof () const             {return _ib->eof();}

	private:
		InputBuffer *_ib;
};

#endif

/*************************************************************************
** CLCommandLine.hpp                                                    **
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

#ifndef CL_COMMANDLINE_HPP
#define CL_COMMANDLINE_HPP

#include <array>
#include <ostream>
#include <string>
#include <sstream>
#include <vector>
#include "CLOption.hpp"
#include "MessageException.hpp"

namespace CL {

class CommandLine {
	public:
		CommandLine (const char *summary, const char *usage, const char *copyright);
		virtual ~CommandLine () =default;
		void parse (int argc, char **argv);
		void help (std::ostream &os, int mode=0) const;
		const std::vector<std::string>& filenames () const {return _files;}

	protected:
		typedef std::pair<Option*,int> OptSectPair;
		void parseShortOption (std::istringstream &is, int argc, char **argv, int &argn);
		void parseLongOption (std::istream &is);
		virtual std::vector<OptSectPair>& options () const =0;
		virtual const char* section (size_t n) const {return nullptr;}
		Option* lookupOption (char optchar) const;
		std::vector<Option*> lookupOption (const std::string &optname) const;

	private:
		const char *_summary;
		const char *_usage;
		const char *_copyright;
		std::vector<std::string> _files;
};


struct CommandLineException : public MessageException
{
	CommandLineException (const std::string &msg) : MessageException(msg) {}
};

} // namespace CL

#endif


/*************************************************************************
** dvisvgm.cpp                                                          **
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

#include <config.h>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "gzstream.h"
#include "CommandLine.h"
#include "DVIToSVG.h"
#include "DVIToSVGActions.h"
#include "EPSToSVG.h"
#include "FileFinder.h"
#include "FilePath.h"
#include "FileSystem.h"
#include "Font.h"
#include "FontCache.h"
#include "FontEngine.h"
#include "FontMap.h"
#include "Ghostscript.h"
#include "HtmlSpecialHandler.h"
#include "InputReader.h"
#include "Message.h"
#include "PageSize.h"
#include "PSInterpreter.h"
#include "SignalHandler.h"
#include "SpecialManager.h"
#include "SVGOutputBase.h"
#include "System.h"

#ifdef __MSVC__
#include <potracelib.h>
#else
extern "C" {
#include <potracelib.h>
}
#endif

using namespace std;

class SVGOutput : public SVGOutputBase
{
	public:
		SVGOutput (const char *base=0, string pattern="", int zip_level=0)
			: _path(base ? base : ""),
			_pattern(pattern),
			_stdout(base == 0),
			_zipLevel(zip_level),
			_page(-1),
			_os(0) {}


		~SVGOutput () {
			delete _os;
		}


		/** Returns an output stream for the given page.
		 *  @param[in] page number of current page
		 *  @param[in] numPages total number of pages in the DVI file
		 *  @return output stream for the given page */
		ostream& getPageStream (int page, int numPages) const {
			string fname = filename(page, numPages);
			if (fname.empty()) {
				delete _os;
				_os = 0;
				return cout;
			}
			if (page == _page)
				return *_os;

			_page = page;
			delete _os;

			if (_zipLevel > 0)
				_os = new ogzstream(fname.c_str(), _zipLevel);
			else
				_os = new ofstream(fname.c_str());
			if (!_os || !*_os) {
				delete _os;
				_os = 0;
				throw MessageException("can't open file "+fname+" for writing");
			}
			return *_os;
		}


		/** Returns the name of the SVG file containing the given page.
		 *  @param[in] page number of current page 
		 *  @param[in] numPages total number of pages */
		string filename (int page, int numPages) const {
			if (_stdout)
				return "";
			string fname = _pattern;
			if (fname.empty())
				fname = numPages > 1 ? "%f-%p" : "%f";
			else if (numPages > 1 && fname.find("%p") == string::npos)
				fname += FileSystem::isDirectory(fname.c_str()) ? "/%f-%p" : "-%p";

			// replace pattern variables by their actual values
			// %f: basename of the DVI file
			// %p: current page number
			ostringstream oss;
			oss << setfill('0') << setw(max(2, int(1+log10((double)numPages)))) << page;
			size_t pos=0;
			while ((pos = fname.find('%', pos)) != string::npos && pos < fname.length()-1) {
				switch (fname[pos+1]) {
					case 'f': fname.replace(pos, 2, _path.basename());  pos += _path.basename().length(); break;
					case 'p': fname.replace(pos, 2, oss.str()); pos += oss.str().length(); break;
					default : ++pos;
				}
			}
			FilePath outpath(fname, true);
			if (outpath.suffix().empty())
				outpath.suffix(_zipLevel > 0 ? "svgz" : "svg");
			string apath = outpath.absolute();
			string rpath = outpath.relative();
			return apath.length() < rpath.length() ? apath : rpath;
		}

	private:
		FilePath _path;
		string _pattern;
		bool _stdout;
		int _zipLevel;
		mutable int _page; // number of current page being written
		mutable ostream *_os;
};

////////////////////////////////////////////////////////////////////////////////

static void show_help (const CommandLine &cmd) {
	cout << PACKAGE_STRING "\n\n";
	cmd.help(cmd.help_arg());
   cout << "\nCopyright (C) 2005-2014 Martin Gieseking <martin.gieseking@uos.de> \n\n";
}


static string remove_path (string fname) {
	fname = FileSystem::adaptPathSeperators(fname);
	size_t slashpos = fname.rfind('/');
	if (slashpos == string::npos)
		return fname;
	return fname.substr(slashpos+1);
}


static string ensure_suffix (string fname, bool eps) {
	size_t dotpos = remove_path(fname).rfind('.');
	if (dotpos == string::npos)
		fname += (eps ? ".eps" : ".dvi");
	return fname;
}


static string get_transformation_string (const CommandLine &args) {
	ostringstream oss;
	if (args.rotate_given())
		oss << 'R' << args.rotate_arg() << ",w/2,h/2";
	if (args.translate_given())
		oss << 'T' << args.translate_arg();
	if (args.scale_given())
		oss << 'S' << args.scale_arg();
	if (args.transform_given())
		oss << args.transform_arg();
	return oss.str();
}


static void set_libgs (CommandLine &args) {
#if !defined(DISABLE_GS) && !defined(HAVE_LIBGS)
	if (args.libgs_given())
		Ghostscript::LIBGS_NAME = args.libgs_arg();
	else if (getenv("LIBGS"))
		Ghostscript::LIBGS_NAME = getenv("LIBGS");
#ifdef MIKTEX
	else {
#ifdef __WIN64__
		const char *gsdll = "mgsdll64.dll";
#else
		const char *gsdll = "mgsdll32.dll";
#endif
		// try to lookup the Ghostscript DLL coming with MiKTeX
		if (const char *gsdll_path = FileFinder::lookup(gsdll))
			Ghostscript::LIBGS_NAME = gsdll_path;
	}
#endif
#endif
}


static bool set_cache_dir (const CommandLine &args) {
	if (args.cache_given() && !args.cache_arg().empty()) {
		if (args.cache_arg() == "none")
			PhysicalFont::CACHE_PATH = 0;
		else if (FileSystem::exists(args.cache_arg().c_str()))
			PhysicalFont::CACHE_PATH = args.cache_arg().c_str();
		else
			Message::wstream(true) << "cache directory '" << args.cache_arg() << "' does not exist (caching disabled)\n";
	}
	else {
		if (const char *userdir = FileSystem::userdir()) {
			static string path = userdir;
			path += "/.dvisvgm";
			path = FileSystem::adaptPathSeperators(path);
			const string cachepath = path+"/cache";
			if (!FileSystem::exists(cachepath.c_str())) {
				if (!FileSystem::exists(path.c_str()))
					FileSystem::mkdir(cachepath.c_str());
				else {
					FileSystem::mkdir(cachepath.c_str());
					// move existing cache files from former location to new one
					vector<string> files;
					FileSystem::collect(path.c_str(), files);
					FORALL(files, vector<string>::iterator, it) {
						if (it->at(0) == 'f' && it->length() > 4 && it->substr(it->length()-4, 4) == ".fgd") {
							const char *fname = it->c_str()+1;
							FileSystem::copy(path+"/"+fname, cachepath+"/"+fname, true);
						}
					}
				}
			}
			path = cachepath;
			PhysicalFont::CACHE_PATH = path.c_str();
		}
		if (args.cache_given() && args.cache_arg().empty()) {
			cout << "cache directory: " << (PhysicalFont::CACHE_PATH ? PhysicalFont::CACHE_PATH : "(none)") << '\n';
			FontCache::fontinfo(PhysicalFont::CACHE_PATH, cout, true);
			return false;
		}
	}
	return true;
}


static bool check_bbox (const string &bboxstr) {
	const char *formats[] = {"none", "min", "dvi", 0};
	for (const char **p=formats; *p; ++p)
		if (bboxstr == *p)
			return true;
	if (isalpha(bboxstr[0])) {
		try {
			PageSize size(bboxstr);
			return true;
		}
		catch (const PageSizeException &e) {
			Message::estream(true) << "invalid bounding box format '" << bboxstr << "'\n";
			return false;
		}
	}
	try {
		BoundingBox bbox;
		bbox.set(bboxstr);
		return true;
	}
	catch (const MessageException &e) {
		Message::estream(true) << e.what() << '\n';
		return false;
	}
}


static void print_version (bool extended) {
	ostringstream oss;
	oss << PACKAGE_STRING;
	if (extended) {
		if (strlen(TARGET_SYSTEM) > 0)
			oss << " (" TARGET_SYSTEM ")";
		int len = oss.str().length();
		oss << "\n" << string(len, '-') << "\n"
			"freetype:    " << FontEngine::version() << "\n";

		Ghostscript gs;
		string gsver = gs.revision(true);
		if (!gsver.empty())
			oss << "Ghostscript: " << gsver + "\n";
		oss <<
#ifdef MIKTEX
			"MiKTeX:      " << FileFinder::version() << "\n"
#else
			"kpathsea:    " << FileFinder::version() << "\n"
#endif
			"potrace:     " << (strchr(potrace_version(), ' ') ? strchr(potrace_version(), ' ')+1 : "unknown") << "\n"
			"zlib:        " << zlibVersion();
	}
	cout << oss.str() << endl;
}


static void init_fontmap (const CommandLine &args) {
	const char *mapseq = args.fontmap_given() ? args.fontmap_arg().c_str() : 0;
	bool additional = mapseq && strchr("+-=", *mapseq);
	if (!mapseq || additional) {
		const char *mapfiles[] = {"ps2pk.map", "dvipdfm.map", "psfonts.map", 0};
		bool found = false;
		for (const char **p=mapfiles; *p && !found; p++)
			found = FontMap::instance().read(*p);
		if (!found)
			Message::wstream(true) << "none of the default map files could be found";
	}
	if (mapseq)
		FontMap::instance().read(mapseq);
}


int main (int argc, char *argv[]) {
	CommandLine args;
	args.parse(argc, argv);
	if (args.error())
		return 1;

	Message::COLORIZE = args.color_given();

	try {
		FileFinder::init(argv[0], "dvisvgm", !args.no_mktexmf_given());
	}
	catch (MessageException &e) {
		Message::estream(true) << e.what() << '\n';
		return 0;
	}

	set_libgs(args);
	if (args.version_given()) {
		print_version(args.version_arg());
		return 0;
	}
	if (args.list_specials_given()) {
		SVGOutput out;
		DVIToSVG dvisvg(cin, out);
		if (const SpecialManager *sm = dvisvg.setProcessSpecials())
			sm->writeHandlerInfo(cout);
		return 0;
	}

	if (!set_cache_dir(args))
		return 0;

	if (argc == 1 || args.help_given()) {
		show_help(args);
		return 0;
	}

	if (argc > 1 && args.numFiles() < 1) {
		Message::estream(true) << "no input file given\n";
		return 1;
	}

	if (args.stdout_given() && args.zip_given()) {
		Message::estream(true) << "writing SVGZ files to stdout is not supported\n";
		return 1;
	}

	if (!check_bbox(args.bbox_arg()))
		return 1;

	if (args.progress_given()) {
		DVIReader::COMPUTE_PAGE_LENGTH = args.progress_given();
		DVIToSVGActions::PROGRESSBAR_DELAY = args.progress_arg();
	}
	SVGTree::CREATE_STYLE = !args.no_styles_given();
	SVGTree::USE_FONTS = !args.no_fonts_given();
	SVGTree::CREATE_USE_ELEMENTS = args.no_fonts_arg() < 1;
	DVIToSVG::TRACE_MODE = args.trace_all_given() ? (args.trace_all_arg() ? 'a' : 'm') : 0;
	Message::LEVEL = args.verbosity_arg();
	PhysicalFont::EXACT_BBOX = args.exact_given();
	PhysicalFont::KEEP_TEMP_FILES = args.keep_given();
	PhysicalFont::METAFONT_MAG = args.mag_arg();
	XMLString::DECIMAL_PLACES = max(0, min(6, args.precision_arg()));
	if (!HtmlSpecialHandler::setLinkMarker(args.linkmark_arg()))
		Message::wstream(true) << "invalid argument '"+args.linkmark_arg()+"' supplied for option --linkmark\n";
	double start_time = System::time();
	bool eps_given=false;
#if defined(HAVE_LIBGS) || !defined(DISABLE_GS)
	eps_given = args.eps_given();
#endif
	string inputfile = ensure_suffix(args.file(0), eps_given);
	ifstream ifs(inputfile.c_str(), ios_base::binary|ios_base::in);
	if (!ifs) {
		Message::estream(true) << "can't open file '" << inputfile << "' for reading\n";
		return 0;
	}
	try {
		SVGOutput out(args.stdout_given() ? 0 : inputfile.c_str(), args.output_arg(), args.zip_given() ? args.zip_arg() : 0);
		SignalHandler::instance().start();
#if defined(HAVE_LIBGS) || !defined(DISABLE_GS)
		if (args.eps_given()) {
			EPSToSVG eps2svg(inputfile, out);
			eps2svg.convert();
			Message::mstream().indent(0);
			Message::mstream(false, Message::MC_PAGE_NUMBER)
				<< "file converted in " << (System::time()-start_time) << " seconds\n";
		}
		else
#endif
		{
			DVIToSVG dvi2svg(ifs, out);
			const char *ignore_specials = args.no_specials_given() ? (args.no_specials_arg().empty() ? "*" : args.no_specials_arg().c_str()) : 0;
			dvi2svg.setProcessSpecials(ignore_specials, true);
			dvi2svg.setPageTransformation(get_transformation_string(args));
			dvi2svg.setPageSize(args.bbox_arg());

			init_fontmap(args);
			pair<int,int> pageinfo;
			dvi2svg.convert(args.page_arg(), &pageinfo);
			Message::mstream().indent(0);
			Message::mstream(false, Message::MC_PAGE_NUMBER) << "\n" << pageinfo.first << " of " << pageinfo.second << " page";
			if (pageinfo.second > 1)
				Message::mstream(false, Message::MC_PAGE_NUMBER) << 's';
			Message::mstream(false, Message::MC_PAGE_NUMBER) << " converted in " << (System::time()-start_time) << " seconds\n";
		}
	}
	catch (DVIException &e) {
		Message::estream() << "\nDVI error: " << e.what() << '\n';
	}
	catch (PSException &e) {
		Message::estream() << "\nPostScript error: " << e.what() << '\n';
	}
	catch (SignalException &e) {
		Message::wstream(true) << "execution interrupted by user\n";
	}
	catch (MessageException &e) {
		Message::estream(true) << e.what() << '\n';
	}
	FileFinder::finish();
	return 0;
}


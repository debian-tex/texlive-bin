/*************************************************************************
** FileSystem.cpp                                                       **
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
#include <cstdlib>
#include <cstring>
#include <fstream>
#include "FileSystem.h"
#include "macros.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif


using namespace std;

#ifdef __WIN32__
	#include <direct.h>
	#include <windows.h>
	const char *FileSystem::DEVNULL = "nul";
	const char FileSystem::PATHSEP = '\\';
	#define unlink _unlink
#else
	#include <dirent.h>
	#include <pwd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	const char *FileSystem::DEVNULL = "/dev/null";
	const char FileSystem::PATHSEP = '/';
#endif


bool FileSystem::remove (const string &fname) {
	return unlink(fname.c_str()) == 0;
}


/** Copies a file.
 *  @param[in] src path of file to copy
 *  @param[in] dest path of target file
 *  @param[in] remove_src remove file 'src' if true
 *  @return true on success */
bool FileSystem::copy (const string &src, const string &dest, bool remove_src) {
	ifstream ifs(src.c_str(), ios::in|ios::binary);
	ofstream ofs(dest.c_str(), ios::out|ios::binary);
	if (ifs && ofs) {
		ofs << ifs.rdbuf();
		if (!ifs.fail() && !ofs.fail() && remove_src) {
			ofs.close();
			ifs.close();
			return remove(src);
		}
		else
			return !remove_src;
	}
	return false;
}


bool FileSystem::rename (const string &oldname, const string &newname) {
	return ::rename(oldname.c_str(), newname.c_str()) == 0;
}


UInt64 FileSystem::filesize (const string &fname) {
#ifdef __WIN32__
	// unfortunately, stat doesn't work properly under Windows
	// so we have to use this freaky code
	WIN32_FILE_ATTRIBUTE_DATA attr;
	GetFileAttributesExA(fname.c_str(), GetFileExInfoStandard, &attr);
	return (static_cast<UInt64>(attr.nFileSizeHigh) << (8*sizeof(attr.nFileSizeLow))) | attr.nFileSizeLow;
#else
	struct stat attr;
	return (stat(fname.c_str(), &attr) == 0) ? attr.st_size : 0;
#endif
}


string FileSystem::adaptPathSeperators (string path) {
	for (size_t i=0; i < path.length(); i++)
		if (path[i] == PATHSEP)
			path[i] = '/';
	return path;
}


string FileSystem::getcwd () {
	char buf[1024];
#ifdef __WIN32__
	return adaptPathSeperators(_getcwd(buf, 1024));
#else
	return ::getcwd(buf, 1024);
#endif
}


/** Changes the work directory.
 *  @param[in] dir path to new work directory
 *  @return true on success */
bool FileSystem::chdir (const std::string &dirname) {
	bool success = false;
	if (const char *cdirname = dirname.c_str()) {
#ifdef __WIN32__
		success = (_chdir(cdirname) == 0);
#else
		success = (chdir(cdirname) == 0);
#endif
	}
	return success;
}


/** Returns the user's home directory. */
const char* FileSystem::userdir () {
#ifdef __WIN32__
	const char *drive=getenv("HOMEDRIVE");
	const char *path=getenv("HOMEPATH");
	if (drive && path) {
		static string ret = string(drive)+path;
		if (!ret.empty())
			return ret.c_str();
	}
	return 0;
#else
	const char *dir=getenv("HOME");
	if (!dir) {
		if (const char *user=getenv("USER")) {
			if (struct passwd *pw=getpwnam(user))
				dir = pw->pw_dir;
		}
	}
	return dir;
#endif
}


/** Private wrapper function for mkdir: creates a single folder.
 *  @param[in] dir folder name
 *  @return true on success */
static bool s_mkdir (const string &dirname) {
	bool success = true;
	if (!FileSystem::exists(dirname)) {
#ifdef __WIN32__
		success = (_mkdir(dirname.c_str()) == 0);
#else
		success = (mkdir(dirname.c_str(), 0775) == 0);
#endif
	}
	return success;
}


static bool inline s_rmdir (const string &dirname) {
#ifdef __WIN32__
	return (_rmdir(dirname.c_str()) == 0);
#else
	return (rmdir(dirname.c_str()) == 0);
#endif
}


/** Removes leading and trailing whitespace from a string. */
static string trim (const string &str) {
	int first=0, last=str.length()-1;
	while (isspace(str[first]))
		first++;
	while (isspace(str[last]))
		last--;
	return str.substr(first, last-first+1);
}


/** Creates a new folder relative to the current work directory. If necessary,
 *  the parent folders are also created.
 *  @param[in] dir single folder name or path to folder
 *  @return true if folder(s) could be created */
bool FileSystem::mkdir (const string &dirname) {
	bool success = false;
	if (const char *cdirname = dirname.c_str()) {
		success = true;
		const string dirstr = adaptPathSeperators(trim(cdirname));
		for (size_t pos=1; success && (pos = dirstr.find('/', pos)) != string::npos; pos++)
			success &= s_mkdir(dirstr.substr(0, pos));
		success &= s_mkdir(dirstr);
	}
	return success;
}


/** Removes a directory and its contents.
 *  @param[in] dirname path to directory
 *  @return true on success */
bool FileSystem::rmdir (const string &dirname) {
	bool ok = false;
	if (isDirectory(dirname)) {
		ok = true;
#ifdef __WIN32__
		string pattern = dirname + "/*";
		WIN32_FIND_DATA data;
		HANDLE h = FindFirstFile(pattern.c_str(), &data);
		bool ready = (h == INVALID_HANDLE_VALUE);
		while (!ready && ok) {
			const char *fname = data.cFileName;
			string path = dirname + "/" + fname;
			if (isDirectory(path)) {
				if (strcmp(fname, ".") != 0 && strcmp(fname, "..") != 0)
					ok = rmdir(path) && s_rmdir(path);
			}
			else if (isFile(path))
				ok = remove(path);
			else
				ok = false;
			ready = !FindNextFile(h, &data);
		}
		FindClose(h);
#else
		if (DIR *dir = opendir(dirname.c_str())) {
			struct dirent *ent;
			while ((ent = readdir(dir)) && ok) {
				const char *fname = ent->d_name;
				string path = string(fname) + "/" + fname;
				if (isDirectory(path)) {
					if (strcmp(fname, ".") != 0 && strcmp(fname, "..") != 0)
						ok = rmdir(path) && s_rmdir(path);
				}
				else if (isFile(path))
					ok = remove(path);
				else
					ok = false;
			}
			closedir(dir);
		}
#endif
		ok = s_rmdir(dirname);
	}
	return ok;
}


/** Checks if a file or directory exits. */
bool FileSystem::exists (const string &fname) {
	if (const char *cfname = fname.c_str()) {

#ifdef __WIN32__
		return GetFileAttributes(cfname) != INVALID_FILE_ATTRIBUTES;
#else
		struct stat attr;
		return stat(cfname, &attr) == 0;
#endif
	}
	return false;
}


/** Returns true if 'fname' references a directory. */
bool FileSystem::isDirectory (const string &fname) {
	if (const char *cfname = fname.c_str()) {
#ifdef __WIN32__
		return GetFileAttributes(cfname) & FILE_ATTRIBUTE_DIRECTORY;
#else
		struct stat attr;
		return stat(cfname, &attr) == 0 && S_ISDIR(attr.st_mode);
#endif
	}
	return false;
}


/** Returns true if 'fname' references a file. */
bool FileSystem::isFile (const string &fname) {
	if (const char *cfname = fname.c_str()) {
#ifdef __WIN32__
		ifstream ifs(cfname);
		return (bool)ifs;
#else
		struct stat attr;
		return stat(cfname, &attr) == 0 && S_ISREG(attr.st_mode);
#endif
	}
	return false;
}


int FileSystem::collect (const char *dirname, vector<string> &entries) {
	entries.clear();
#ifdef __WIN32__
	string pattern = string(dirname) + "/*";
	WIN32_FIND_DATA data;
	HANDLE h = FindFirstFile(pattern.c_str(), &data);
	bool ready = (h == INVALID_HANDLE_VALUE);
	while (!ready) {
		string fname = data.cFileName;
		string path = string(dirname)+"/"+fname;
		string typechar = isFile(path) ? "f" : isDirectory(path) ? "d" : "?";
		if (fname != "." && fname != "..")
			entries.push_back(typechar+fname);
		ready = !FindNextFile(h, &data);
	}
	FindClose(h);
#else
	if (DIR *dir = opendir(dirname)) {
		struct dirent *ent;
		while ((ent = readdir(dir))) {
			string fname = ent->d_name;
			string path = string(dirname)+"/"+fname;
			string typechar = isFile(path) ? "f" : isDirectory(path) ? "d" : "?";
			if (fname != "." && fname != "..")
				entries.push_back(typechar+fname);
		}
		closedir(dir);
	}
#endif
	return entries.size();
}



//========================================================================
//
// gfile.h
//
// Miscellaneous file and directory name manipulation.
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef GFILE_H
#define GFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#if defined(_WIN32)
#  include <sys/stat.h>
#  ifdef FPTEX
#    include <win32lib.h>
#  else
#    include <windows.h>
#  endif
#elif defined(ACORN)
#elif defined(MACOS)
#  include <ctime.h>
#elif defined(ANDROID)
#else
#  include <unistd.h>
#  include <sys/types.h>
#  ifdef VMS
#    include "vms_dirent.h"
#  elif HAVE_DIRENT_H
#    include <dirent.h>
#    define NAMLEN(d) strlen((d)->d_name)
#  else
#    define dirent direct
#    define NAMLEN(d) (d)->d_namlen
#    if HAVE_SYS_NDIR_H
#      include <sys/ndir.h>
#    endif
#    if HAVE_SYS_DIR_H
#      include <sys/dir.h>
#    endif
#    if HAVE_NDIR_H
#      include <ndir.h>
#    endif
#  endif
#endif
#include "gtypes.h"

class GString;

//------------------------------------------------------------------------

// Get home directory path.
extern GString *getHomeDir();

// Get current directory.
extern GString *getCurrentDir();

// Append a file name to a path string.  <path> may be an empty
// string, denoting the current directory).  Returns <path>.
extern GString *appendToPath(GString *path, const char *fileName);

// Grab the path from the front of the file name.  If there is no
// directory component in <fileName>, returns an empty string.
extern GString *grabPath(char *fileName);

// Is this an absolute path or file name?
extern GBool isAbsolutePath(char *path);

// Make this path absolute by prepending current directory (if path is
// relative) or prepending user's directory (if path starts with '~').
extern GString *makePathAbsolute(GString *path);

// Get the modification time for <fileName>.  Returns 0 if there is an
// error.
extern time_t getModTime(char *fileName);

// Create a temporary file and open it for writing.  If <ext> is not
// NULL, it will be used as the file name extension.  Returns both the
// name and the file pointer.  For security reasons, all writing
// should be done to the returned file pointer; the file may be
// reopened later for reading, but not for writing.  The <mode> string
// should be "w" or "wb".  Returns true on success.
extern GBool openTempFile(GString **name, FILE **f,
			  const char *mode, const char *ext);

// Create a directory.  Returns true on success.
extern GBool createDir(char *path, int mode);

// Execute <command>.  Returns true on success.
extern GBool executeCommand(char *cmd);

#ifdef _WIN32
// Convert a file name from Latin-1 to UTF-8.
extern GString *fileNameToUTF8(char *path);

// Convert a file name from UCS-2 to UTF-8.
extern GString *fileNameToUTF8(wchar_t *path);
#endif

// Open a file.  On Windows, this converts the path from UTF-8 to
// UCS-2 and calls _wfopen (if available).  On other OSes, this simply
// calls fopen.
extern FILE *openFile(const char *path, const char *mode);

// Just like fgets, but handles Unix, Mac, and/or DOS end-of-line
// conventions.
extern char *getLine(char *buf, int size, FILE *f);

// Type used by gfseek/gftell for file offsets.  This will be 64 bits
// on systems that support it.
#if HAVE_FSEEKO
typedef off_t GFileOffset;
#define GFILEOFFSET_MAX 0x7fffffffffffffffLL
#elif HAVE_FSEEK64
typedef long long GFileOffset;
#define GFILEOFFSET_MAX 0x7fffffffffffffffLL
#elif HAVE_FSEEKI64
typedef __int64 GFileOffset;
#define GFILEOFFSET_MAX 0x7fffffffffffffffLL
#else
typedef long GFileOffset;
#define GFILEOFFSET_MAX LONG_MAX
#endif

// Like fseek, but uses a 64-bit file offset if available.
extern int gfseek(FILE *f, GFileOffset offset, int whence);

// Like ftell, but returns a 64-bit file offset if available.
extern GFileOffset gftell(FILE *f);

//------------------------------------------------------------------------
// GDir and GDirEntry
//------------------------------------------------------------------------

class GDirEntry {
public:

  GDirEntry(char *dirPath, char *nameA, GBool doStat);
  ~GDirEntry();
  GString *getName() { return name; }
  GBool isDir() { return dir; }

private:

  GString *name;		// dir/file name
  GBool dir;			// is it a directory?
};

class GDir {
public:

  GDir(char *name, GBool doStatA = gTrue);
  ~GDir();
  GDirEntry *getNextEntry();
  void rewind();

private:

  GString *path;		// directory path
  GBool doStat;			// call stat() for each entry?
#if defined(_WIN32)
  WIN32_FIND_DATAA ffd;
  HANDLE hnd;
#elif defined(ACORN)
#elif defined(MACOS)
#elif defined(ANDROID)
#else
  DIR *dir;			// the DIR structure from opendir()
#ifdef VMS
  GBool needParent;		// need to return an entry for [-]
#endif
#endif
};

#endif

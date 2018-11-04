/*
 * aconf-w32.h
 *
 * Copyright 2002-2003 Glyph & Cog, LLC
 * Copyright 2018 A. Kakuto
 */

#ifndef ACONF_H
#define ACONF_H

#include <aconf2.h>

/*
 * Use A4 paper size instead of Letter for PostScript output.
 */
#define A4_PAPER 1

/*
 * Do not allow text selection.
 */
#undef NO_TEXT_SELECT

/*
 * Include support for OPI comments.
 */
#undef OPI_SUPPORT

/*
 * Enable multithreading support.
 */
#define MULTITHREADED 1

/*
 * Enable C++ exceptions.
 */
#define USE_EXCEPTIONS 1

/*
 * Use fixed point (instead of floating point) arithmetic.
 */
#undef USE_FIXEDPOINT

/*
 * Directory with the Xpdf app-defaults file.
 */
#undef APPDEFDIR

/*
 * Full path for the system-wide xpdfrc file.
 */
#undef SYSTEM_XPDFRC

/*
 * Various include files and functions.
 */
#undef HAVE_DIRENT_H
#undef HAVE_SYS_NDIR_H
#undef HAVE_SYS_DIR_H
#undef HAVE_NDIR_H
#undef HAVE_SYS_SELECT_H
#undef HAVE_SYS_BSDTYPES_H
#undef HAVE_STRINGS_H
#undef HAVE_BSTRING_H
#define HAVE_POPEN 1

#ifdef __cplusplus
extern "C" {
#endif
#include <kpathsea/config.h>
#ifdef __cplusplus
}
#endif

#undef index

#undef HAVE_MKSTEMP
#undef HAVE_MKSTEMPS
#undef SELECT_TAKES_INT
#define HAVE_STD_SORT 1
#undef HAVE_FSEEKO
#undef HAVE_FSEEK64
#define HAVE_FSEEKI64 1
#undef _FILE_OFFSET_BITS
#undef _LARGE_FILES
#undef _LARGEFILE_SOURCE
#undef HAVE_XTAPPSETEXITFLAG

/*
 * This is defined if using libXpm.
 */
#undef HAVE_X11_XPM_H

/*
 * One of these is defined if using FreeType (version 1 or 2).
 */
#undef HAVE_FREETYPE_H
#define HAVE_FREETYPE_FREETYPE_H 1

/*
 * This is defined if using FreeType version 2.
 */
#define FREETYPE2 1

/*
 * This is defined if using libpaper.
 */
#undef HAVE_PAPER_H

/*
 * Enable support for loading plugins.
 */
#undef ENABLE_PLUGINS

/*
 * Defined if the Splash library is avaiable.
 */
#define HAVE_SPLASH 1

/*
 * Enable support for CMYK output.
 */
#define SPLASH_CMYK 1

#ifdef _WIN32
#ifdef LINKDLL
#define XPDFDLL __declspec(dllimport)
#else
#define XPDFDLL extern
#endif
#ifdef __cplusplus
extern "C" {
#endif
XPDFDLL
FILE *fsyscp_fopen (const char *filename, const char *mode);
#ifdef __cplusplus
}
#endif
#undef fopen
#define fopen fsyscp_fopen
#endif /* _WIN32 */

#endif

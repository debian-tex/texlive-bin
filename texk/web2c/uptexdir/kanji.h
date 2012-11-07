/*
   kanji.h: Handling 2byte char, and so on.
*/
#ifndef KANJI_H
#define KANJI_H
#include "cpascal.h"
#include <ptexenc/ptexenc.h>
#include <ptexenc/unicode.h>
#ifdef eupTeX
#define getintone(w) ((w).cint1)
#define setintone(w,a) ((w).cint1=(a))
#endif

#define KANJI

#if !defined(WIN32)
extern int sjisterminal;
#endif

/* functions */
#define XXHi(x) BYTE1(x)
#define XHi(x) BYTE2(x)
#define Hi(x) BYTE3(x)
#define Lo(x) BYTE4(x)

extern boolean check_kanji (integer c);
#define checkkanji check_kanji
extern boolean is_char_ascii (integer c);
#define ischarascii is_char_ascii
extern boolean is_char_kanji (integer c);
#define ischarkanji is_char_kanji
extern boolean ismultiprn (integer c);
extern integer calc_pos (integer c);
#define calcpos calc_pos
extern integer kcatcodekey (integer c);
extern integer multilenbuffchar (integer c);

extern void init_default_kanji (const_string file_str, const_string internal_str);
/* for upTeX, e-upTeX, upBibTeX, upDVItype, upPLtoTF, and upTFtoPL */
#define initkanji() init_default_kanji("utf8", "uptex")
/* for upDVItype */
#define setpriorfileenc() set_prior_file_enc()

/* number of rest of multi-char for kcode_pos */
#define nrestmultichr(x)  ( (x)!=0 ? ((x) / 8) + 2 - ((x) % 8) : -1 )

#ifndef PRESERVE_PUTC
#undef putc
#define putc(c,fp) putc2(c,fp)
#endif /* !PRESERVE_PUTC */

#ifdef UPBIBTEX
#define inputline2(fp,buff,pos,size,ptr) input_line2(fp,buff,pos,size,ptr)
#else
#define inputline2(fp,buff,pos,size) input_line2(fp,buff,pos,size,NULL)
#endif

extern void init_kanji (const_string file_str, const_string internal_str);
extern void dump_kanji (FILE *fp);
extern void undump_kanji (FILE *fp);
#define dumpkanji dump_kanji
#define undumpkanji undump_kanji

#endif /* not KANJI_H */

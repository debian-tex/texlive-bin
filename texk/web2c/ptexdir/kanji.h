/*
   kanji.h: Handling 2byte char, and so on.
*/
#ifndef KANJI_H
#define KANJI_H
#include "cpascal.h"
#include <ptexenc/ptexenc.h>
#ifdef epTeX
#include <ptexenc/unicode.h>
#define getintone(w) ((w).cint1)
#define setintone(w,a) ((w).cint1=(a))
#endif

#ifndef KANJI
#define KANJI
#endif

/* allow file names with 0x5c in (e)pTeX on windows */
#if defined(WIN32)
#include <kpathsea/knj.h>
#define not_kanji_char_seq(a,b) (!(is_cp932_system && isknj(a) && isknj2(b)))
#else
#define not_kanji_char_seq(a,b) (1)
#endif
#define notkanjicharseq not_kanji_char_seq

#if !defined(WIN32)
extern int sjisterminal;
#endif

/* functions */
#define Hi(x) (((x) >> 8) & 0xff)
#define Lo(x) ((x) & 0xff)

extern int check_kanji (integer c);
#define checkkanji check_kanji
extern boolean is_char_ascii (integer c);
#define ischarascii is_char_ascii
extern boolean is_char_kanji (integer c);
#define ischarkanji is_char_kanji
extern boolean ismultiprn (integer c);
extern integer calc_pos (integer c);
#define calcpos calc_pos
extern integer kcatcodekey (integer c);

extern void init_default_kanji (const_string file_str, const_string internal_str);
#ifdef PBIBTEX
/* pBibTeX is EUC only */
#define initkanji() init_default_kanji(NULL, "euc")
#elif defined(WIN32)
/* for pTeX, e-pTeX, pDVItype, pPLtoTF, and pTFtoPL */
#define initkanji() init_default_kanji(NULL, "sjis")
#else
#define initkanji() init_default_kanji(NULL, "euc")
#endif
/* for pDVItype */
#define setpriorfileenc() set_prior_file_enc()

#ifndef PRESERVE_PUTC
#undef putc
#define putc(c,fp) putc2(c,fp)
#endif /* !PRESERVE_PUTC */

#ifdef PBIBTEX
#define inputline2(fp,buff,pos,size,ptr) input_line2(fp,buff,pos,size,ptr)
#else
#define inputline2(fp,buff,pos,size) input_line2(fp,buff,pos,size,NULL)
#endif

#endif /* not KANJI_H */

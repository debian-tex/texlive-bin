/* eptexextra.h: banner etc. for e-pTeX.

   This is included by e-pTeX, from eptexextra.c
*/

#include <eptexdir/etex_version.h> /* for ETEX_VERSION */
#include <eptexdir/eptex_version.h> /* for EPTEX_VERSION */
#include <eptexdir/ptex_version.h> /* for PTEX_VERSION */

#define BANNER "This is e-pTeX, Version 3.1415926-" PTEX_VERSION "-" EPTEX_VERSION "-" ETEX_VERSION
#define COPYRIGHT_HOLDER "D.E. Knuth"
#define AUTHOR NULL
#define PROGRAM_HELP EPTEXHELP
#define BUG_ADDRESS "ptex-staff@ml.asciimw.jp"
#define DUMP_VAR TEXformatdefault
#define DUMP_LENGTH_VAR formatdefaultlength
#define DUMP_OPTION "fmt"
#define DUMP_EXT ".fmt"
#define INPUT_FORMAT kpse_tex_format
#define INI_PROGRAM "inieptex"
#define VIR_PROGRAM "vireptex"

#ifdef Xchr
#undef Xchr
#define Xchr(x) (x)
#endif /* Xchr */

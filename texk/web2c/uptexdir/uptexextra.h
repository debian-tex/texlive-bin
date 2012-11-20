/* uptexextra.h: banner etc. for upTeX.

   This is included by upTeX, from uptexextra.c
*/

#include <ptexdir/ptex_version.h> /* for PTEX_VERSION */
#include <uptexdir/uptex_version.h> /* for UPTEX_VERSION */

#define BANNER "This is upTeX, Version 3.1415926-" PTEX_VERSION "-" UPTEX_VERSION
#define COPYRIGHT_HOLDER "D.E. Knuth"
#define AUTHOR NULL
#define PROGRAM_HELP UPTEXHELP
#define BUG_ADDRESS "ptex-staff@ml.asciimw.jp"
#define DUMP_VAR TEXformatdefault
#define DUMP_LENGTH_VAR formatdefaultlength
#define DUMP_OPTION "fmt"
#define DUMP_EXT ".fmt"
#define INPUT_FORMAT kpse_tex_format
#define INI_PROGRAM "iniuptex"
#define VIR_PROGRAM "viruptex"

#ifdef Xchr
#undef Xchr
#define Xchr(x) (x)
#endif /* Xchr */

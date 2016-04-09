#ifndef MFLUAC_H
#define MFLUAC_H

extern int mfluabeginprogram(void);
extern int mfluaPREstartofMF(void);
extern int mfluaPREmaincontrol(void);
extern int mfluaPOSTmaincontrol(void);
extern int mfluainitialize(void);
extern int mfluaPOSTfinalcleanup(void);
extern int mfluaendprogram(void);

extern int mfluaprintpath(halfword, strnumber, boolean);
extern int mfluarunscript(halfword, halfword, halfword);
extern int mfluaprintedges(strnumber, boolean, integer, integer);

extern int mfluaPREoffsetprep(halfword, halfword); 
extern int mfluaPOSToffsetprep(halfword, halfword); 

extern int mfluaPREfillenveloperhs(halfword);
extern int mfluaPOSTfillenveloperhs(halfword);
extern int mfluaPREfillenvelopelhs(halfword);
extern int mfluaPOSTfillenvelopelhs(halfword);

extern int mfluaPREfillspecrhs(halfword);
extern int mfluaPOSTfillspecrhs(halfword);
extern int mfluaPREfillspeclhs(halfword);
extern int mfluaPOSTfillspeclhs(halfword);

extern int mfluaPREmakechoices(halfword);
extern int mfluaPOSTmakechoices(halfword);

extern int mfluaPREmovetoedges(halfword);
extern int mfluaPOSTmovetoedges(halfword);

extern int mfluaprintretrogradeline(integer, integer, integer, integer);

extern int mfluaprinttransitionlinefrom(integer, integer);
extern int mfluaprinttransitionlineto(integer, integer);

extern int mfluaPREmakeellipse(integer, integer, integer, integer, integer, integer);
extern int mfluaPOSTmakeellipse(integer, integer, integer, integer, integer, integer);

#include <lauxlib.h>
extern int luaopen_kpse(lua_State * L);

#endif /* MFLUAC_H */

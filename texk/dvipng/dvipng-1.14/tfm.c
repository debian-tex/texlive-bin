/* tfm.c */

/************************************************************************

  Part of the dvipng distribution

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this program. If not, see
  <http://www.gnu.org/licenses/>.

  Copyright (C) 2002-2009 Jan-�ke Larsson

************************************************************************/

#include "dvipng.h"

bool ReadTFM(struct font_entry * tfontp, char* tfmname)
{
  struct filemmap fmmap;
  struct char_entry *tcharptr;  
  unsigned char *position; 
  int lh,bc,ec,nw, c;
  dviunits* width;

  DEBUG_PRINT((DEBUG_DVI|DEBUG_FT|DEBUG_TFM),
	      ("\n  OPEN METRICS:\t'%s'", tfmname));
  if (MmapFile(tfmname,&fmmap)) return(false);
  position=(unsigned char*)fmmap.data;
  lh = UNumRead(position+2,2);
  bc = UNumRead(position+4,2);
  ec = UNumRead(position+6,2);
  nw = UNumRead(position+8,2);
  DEBUG_PRINT(DEBUG_TFM,(" %d %d %d %d",lh,bc,ec,nw));
  width=malloc(nw*sizeof(dviunits));  
  c=0;
  position=position+24+(lh+ec-bc+1)*4;
  while( c < nw ) {
    width[c] = SNumRead(position,4);
    c++;
    position += 4; 
  }
  
  /* Read char widths */
  c=bc;
  position=(unsigned char*)fmmap.data+24+lh*4;
  while(c <= ec) {
#ifdef _WIN64
    DEBUG_PRINT(DEBUG_TFM,("\n@%I64d TFM METRICS:\t", 
			   (__int64)position - (__int64)fmmap.data));
#else
    DEBUG_PRINT(DEBUG_TFM,("\n@%ld TFM METRICS:\t", 
			   (long)position - (long)fmmap.data));
#endif
    tcharptr=xmalloc(sizeof(struct char_entry));
    tcharptr->data=NULL;
    tcharptr->tfmw=width[*position];
    DEBUG_PRINT(DEBUG_TFM,("%d [%d] %d",c,*position,tcharptr->tfmw));
    tcharptr->tfmw = (dviunits) 
      ((int64_t) tcharptr->tfmw * tfontp->s / (1 << 20));
    DEBUG_PRINT(DEBUG_TFM,(" (%d)",tcharptr->tfmw));
    if (c >= NFNTCHARS) /* Only positive for now */
      Fatal("tfm file %s exceeds char numbering limit",tfmname);
    tfontp->chr[c] = tcharptr;
    c++;
    position += 4;
  }
  free(width);
  UnMmapFile(&fmmap);
  return(true);
}

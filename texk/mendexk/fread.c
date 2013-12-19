#include "mendex.h"

#include <kpathsea/tex-file.h>
#include <ptexenc/ptexenc.h>

#include "exkana.h"
#include "exvar.h"

#define BUFSIZE 65535

static int getestr(char *buff, char *estr);
static void chkpageattr(struct page *p);

/*   read idx file   */
int idxread(char *filename, int start)
{
	int i,j,k,l,m,n,cc,indent,wflg,flg=0,bflg=0,nest,esc,quo,eflg=0,pacc,preject;
	char buff[BUFSIZE],wbuff[BUFSIZE],estr[256],table[BUFSIZE],*tmp1,*tmp2;
	FILE *fp;

	pacc=acc;
	preject=reject;

	if (filename==NULL) {
		fp=stdin;
#ifdef WIN32
		setmode(fileno(fp), _O_BINARY);
#endif
		verb_printf(efp, "Scanning input file stdin.");
	}
	else {
		if(kpse_in_name_ok(filename))
			fp=nkf_open(filename,"rb");
		else
			fp=NULL;
		if (fp==NULL) {
			sprintf(buff,"%s.idx",filename);
			if(kpse_in_name_ok(buff))
				fp=nkf_open(buff,"rb");
			else
				fp=NULL;
			if (fp==NULL) {
				warn_printf(efp,"Warning: Couldn't find input file %s.\n",filename);
				return 1;
			}
			else strcpy(filename,buff);
		}
		verb_printf(efp,"Scanning input file %s.",filename);
	}

	for (i=start,n=1;;i++,n++) {
		if (!(i%100))
			ind=(struct index *)realloc(ind,sizeof(struct index)*(i+100));
LOOP:
		ind[i].lnum=n;
		if (mfgets(buff,sizeof(buff)-1,fp)==NULL) break;
		for (j=bflg=cc=0;j<strlen(buff);j++) {
			if (buff[j]!=' ' && buff[j]!='\n') cc=1;
			if (bflg==0) {
				if (strncmp(&buff[j],keyword,strlen(keyword))==0) {
					j+=strlen(keyword);
					bflg=1;
				}
			}
			if (bflg==1) {
				if (buff[j]==arg_open) {
					j++;
					break;
				}
			}
		}
		if (j==strlen(buff)) {
			if (cc) reject++;
			i--;
			continue;
		}
		indent=wflg=k=nest=esc=quo=0;

/*   analize words   */

		for (;;j++,k++) {
			if (buff[j]=='\n' || buff[j]=='\0') {
				verb_printf(efp,"\nWarning: Incomplete first argument in %s, line %d.",filename,ind[i].lnum);
				warn++;
				n++;
				goto LOOP;
			}

			if (buff[j]==quote && esc==0 && quo==0) {
				k--;
				quo=1;
				continue;
			}

			if (quo==0 && buff[j]==escape) {
				esc=1;
			}

			if (quo==0) {
				if (esc==0 && buff[j]==arg_open) {
					nest++;
					wbuff[k]=buff[j];
					continue;
				}
				else if (esc==0 && buff[j]==arg_close && nest>0) {
					nest--;
					wbuff[k]=buff[j];
					continue;
				}
				if (nest==0) {
					if (buff[j]==level) {
						esc=0;
						if (indent>=2) {
							fprintf(efp,"\nError: Extra `%c\' in %s, line %d.",level,filename,ind[i].lnum);
							if (efp!=stderr) fprintf(stderr,"\nError: Extra `%c\' in %s, line %d.",level,filename,ind[i].lnum);
							eflg++;
							reject++;
							n++;
							goto LOOP;
						}
						ind[i].idx[indent]=xmalloc(k+1);
						strncpy(ind[i].idx[indent],wbuff,k);
						ind[i].idx[indent][k]='\0';
						if (!wflg) ind[i].org[indent]=NULL;
						indent++;
						wflg=0;
						k= -1;
						continue;
					}
					else if (buff[j]==actual) {
						esc=0;
						if (wflg) {
							fprintf(efp,"\nError: Extra `%c\' in %s, line %d.",actual,filename,ind[i].lnum);
							if (efp!=stderr) fprintf(stderr,"\nError: Extra `%c\' in %s, line %d.",actual,filename,ind[i].lnum);
							eflg++;
							reject++;
							n++;
							goto LOOP;
						}
						ind[i].org[indent]=xmalloc(k+1);
						strncpy(ind[i].org[indent],wbuff,k);
						ind[i].org[indent][k]='\0';
						wflg=1;
						k= -1;
						continue;
					}
					else if ((esc==0 && buff[j]==arg_close) || buff[j]==encap) {
						esc=0;
						if (buff[j]==encap) {
							j++;
							cc=getestr(&buff[j],estr);
							if (cc<0) {
								fprintf(efp,"\nBad encap string in %s, line %d.",filename,ind[i].lnum);
								if (efp!=stderr) fprintf(stderr,"\nBad encap string in %s, line %d.",filename,ind[i].lnum);
								eflg++;
								reject++;
								n++;
								goto LOOP;
							}
							j+=cc;
						}
						else estr[0]='\0';

						ind[i].idx[indent]=xmalloc(k+1);
						strncpy(ind[i].idx[indent],wbuff,k);
						ind[i].idx[indent][k]='\0';
						if (strlen(ind[i].idx[indent])==0) {
							if (wflg) {
								strcpy(ind[i].idx[indent],ind[i].org[indent]);
							}
							else if (indent>0) {
								indent--;
							}
							else {
								verb_printf(efp,"\nWarning: Illegal null field in %s, line %d.",filename,ind[i].lnum);
								warn++;
								n++;
								goto LOOP;
							}
						}
						if (!wflg) {
							ind[i].org[indent]=NULL;
						}
						break;
					}
				}
				if (bcomp==1) {
					if (buff[j]==' ' || buff[j]=='\t') {
						esc=0;
						if (k==0) {
							k--;
							continue;
						}
						else if (buff[j+1]==' ' || buff[j+1]=='\t' || buff[j+1]==encap || buff[j+1]==arg_close || buff[j+1]==actual || buff[j+1]==level) {
							k--;
							continue;
						}
						else if (buff[j]=='\t') {
							wbuff[k]=' ';
							continue;
						}
					}
				}
			}
			else quo=0;

			wbuff[k]=buff[j];
			if (buff[j]!=escape) esc=0;
			if ((unsigned char)buff[j]>=0x80) {
				wbuff[k+1]=buff[j+1];
				j++;
				k++;
			}
		}
		ind[i].words=indent+1;

/*   kana-convert   */

		for (k=0;k<ind[i].words;k++) {
			if (ind[i].org[k]==NULL) {
				cc=convert(ind[i].idx[k],table);
				if (cc==-1) {
					fprintf(efp,"in %s, line %d.",filename,ind[i].lnum);
					if (efp!=stderr) fprintf(stderr,"in %s, line %d.",filename,ind[i].lnum);
					eflg++;
					reject++;
					n++;
					goto LOOP;
				}
				ind[i].dic[k]=xstrdup(table);
			}
			else {
				cc=convert(ind[i].org[k],table);
				if (cc==-1) {
					fprintf(efp,"in %s, line %d.",filename,ind[i].lnum);
					if (efp!=stderr) fprintf(stderr,"in %s, line %d.",filename,ind[i].lnum);
					eflg++;
					reject++;
					n++;
					goto LOOP;
				}
				ind[i].dic[k]=xstrdup(table);
			}
		}
		acc++;

/*   page edit   */

		if (i==0) {
			ind[0].num=0;
			ind[0].p=xmalloc(sizeof(struct page)*16);
			for (;buff[j]!=arg_open && buff[j]!='\n' && buff[j]!='\0';j++);
			if (buff[j]=='\n' || buff[j]=='\0') {
				verb_printf(efp,"\nWarning: Missing second argument in %s, line %d.",filename,ind[i].lnum);
				acc--;
				reject++;
				warn++;
				n++;
				goto LOOP;
			}
			j++;
			for (k=nest=0;;j++,k++) {
				if (buff[j]=='\n' || buff[j]=='\0') {
					verb_printf(efp,"\nWarning: Incomplete second argument in %s, line %d.",filename,ind[i].lnum);
					acc--;
					reject++;
					warn++;
					n++;
					goto LOOP;
				}
				if (buff[j]==arg_open)
					nest++;
				else if (buff[j]==arg_close) {
					if (nest==0) {
						table[k]='\0';	
						ind[0].p[0].page=xstrdup(table);
						break;
					}
					else nest--;
				}
				else if ((unsigned char)buff[j]>=0x80) {
					table[k]=buff[j];
					j++;
					k++;
				}
				table[k]=buff[j];
			}
			ind[0].p[0].enc=xstrdup(estr);
			chkpageattr(&ind[0].p[0]);
		}
		else {
			for (l=0;l<i;l++) {
				flg=0;
				if (ind[i].words!=ind[l].words) continue;
				for (flg=1,m=0;m<ind[i].words;m++) {
					if (strcmp(ind[i].idx[m],ind[l].idx[m])!=0) {
						flg=0;
						break;
					}
					if (strcmp(ind[i].dic[m],ind[l].dic[m])!=0) {
						if (ind[i].org[m]!=NULL) tmp1=ind[i].org[m];
						else tmp1=ind[i].idx[m];

						if (ind[l].org[m]!=NULL) tmp2=ind[l].org[m];
						else tmp2=ind[i].idx[m];

						verb_printf(efp,"\nWarning: Sort key \"%s\" is different from previous key \"%s\" for same index \"%s\" in %s, line %d.",tmp1, tmp2, ind[i].idx[m], filename,ind[i].lnum);
						warn++;
						flg=0;
						break;
					}
				}
				if (flg==1) break;
			}

			if (flg==1) {
				for (m=0;m<ind[i].words;m++) {
					free(ind[i].idx[m]);
					free(ind[i].dic[m]);
				}

				i--;
				for (;buff[j]!=arg_open && buff[j]!='\n' && buff[j]!='\0';j++);
				if (buff[j]=='\n' || buff[j]=='\0') {
					verb_printf(efp,"\nWarning: Missing second argument in %s, line %d.",filename,ind[i].lnum);
					acc--;
					reject++;
					warn++;
					n++;
					i++;
					goto LOOP;
				}
				j++;
				for (k=nest=0;;j++,k++) {
					if (buff[j]=='\n' || buff[j]=='\0') {
						verb_printf(efp,"\nWarning: Incomplete second argument in %s, line %d.",filename,ind[i].lnum);
						warn++;
						n++;
						i++;
						goto LOOP;
					}
					if (buff[j]==arg_open)
						nest++;
					else if (buff[j]==arg_close) {
						if (nest==0) break;
						else nest--;
					}
					else if ((unsigned char)buff[j]>=0x80) {
						table[k]=buff[j];
						j++;
						k++;
					}
					table[k]=buff[j];
				}

				table[k]='\0';	

				for (k=0;k<=ind[l].num;k++) {
					if (strcmp(ind[l].p[k].page,table)==0) {
						if (strcmp(ind[l].p[k].enc,estr)==0) break;
					}
				}

				if (k>ind[l].num) {
					ind[l].num++;
					if (!((ind[l].num)%16)) ind[l].p=(struct page *)realloc(ind[l].p,sizeof(struct page)*((int)((ind[l].num)/16)+1)*16);

					ind[l].p[ind[l].num].page=xstrdup(table);	

					ind[l].p[ind[l].num].enc=xstrdup(estr);	
					chkpageattr(&ind[l].p[ind[l].num]);
				}
			}
			else {
				ind[i].num=0;
				ind[i].p=xmalloc(sizeof(struct page)*16);
				for (;buff[j]!=arg_open && buff[j]!='\n' && buff[j]!='\0';j++);
				if (buff[j]=='\n' || buff[j]=='\0') {
					verb_printf(efp,"\nWarning: Missing second argument in %s, line %d.",filename,ind[i].lnum);
					acc--;
					reject++;
					warn++;
					n++;
					goto LOOP;
				}
				j++;
				for (k=nest=0;;j++,k++) {
					if (buff[j]=='\n' || buff[j]=='\0') {
						verb_printf(efp,"\nWarning: Incomplete second argument in %s, line %d.",filename,ind[i].lnum);
						acc--;
						reject++;
						warn++;
						n++;
						goto LOOP;
					}
					if (buff[j]==arg_open)
						nest++;
					if (buff[j]==arg_close) {
						if (nest==0) {
							table[k]='\0';	
							ind[i].p[0].page=xstrdup(table);	
							break;
						}
						else nest--;
					}
					else if ((unsigned char)buff[j]>=0x80) {
						table[k]=buff[j];
						j++;
						k++;
					}
					table[k]=buff[j];
				}
				ind[l].p[0].enc=xstrdup(estr);	
				chkpageattr(&ind[i].p[0]);
			}
		}
	}
	lines=i;
	if (filename != NULL) nkf_close(fp);

	verb_printf(efp,"...done (%d entries accepted, %d rejected).\n",acc-pacc, reject-preject);
	return eflg;
}

/*   pic up encap string   */
static int getestr(char *buff, char *estr)
{
	int i,nest=0;

	for (i=0;i<strlen(buff);i++) {
		if (buff[i]==encap) {
			if (i>0) {
				if ((unsigned char)buff[i-1]<0x80) {
					estr[i]=buff[i];
					i++;
				}
			}
			else {
				estr[i]=buff[i];
				i++;
			}
		}
		if (nest==0 && buff[i]==arg_close) {
			estr[i]='\0';
			return i;
		}
		if (buff[i]==arg_open) nest++;
		else if (buff[i]==arg_close) nest--;
		estr[i]=buff[i];
		if ((unsigned char)buff[i]>0x80) {
			i++;
			estr[i]=buff[i];
		}
	}

	return -1;
}

static void chkpageattr(struct page *p)
{
	int i,j,cc=0;

	for (i=0;i<strlen(p->page);i++) {
		if (strncmp(page_compositor,&p->page[i],strlen(page_compositor))==0) {
			p->attr[cc]=pattr[cc];
			cc++;
			i+=strlen(page_compositor)-1;
		}
		else {
ATTRLOOP:
			if (!((p->page[i]>='0' && p->page[i]<='9') || (p->page[i]>='A' && p->page[i]<='Z') || (p->page[i]>='a' && p->page[i]<='z'))) {
				p->attr[cc]= -1;
				if (cc<2) p->attr[++cc]= -1;
				return;
			}
			switch(page_precedence[pattr[cc]]) {
			case 'r':
				if (strchr("ivxlcdm",p->page[i])==NULL) {
					pattr[cc]++;
					for (j=cc+1;j<3;j++) pattr[j]=0;
					goto ATTRLOOP;
				}
				break;
			case 'R':
				if (strchr("IVXLCDM",p->page[i])==NULL) {
					pattr[cc]++;
					for (j=cc+1;j<3;j++) pattr[j]=0;
					goto ATTRLOOP;
				}
				break;
			case 'n':
				if (p->page[i]<'0' || p->page[i]>'9') {
					pattr[cc]++;
					for (j=cc+1;j<3;j++) pattr[j]=0;
					goto ATTRLOOP;
				}
				break;
			case 'a':
				if (p->page[i]<'a' || p->page[i]>'z') {
					pattr[cc]++;
					for (j=cc+1;j<3;j++) pattr[j]=0;
					goto ATTRLOOP;
				}
				break;
			case 'A':
				if (p->page[i]<'A' || p->page[i]>'Z') {
					pattr[cc]++;
					for (j=cc+1;j<3;j++) pattr[j]=0;
					goto ATTRLOOP;
				}
				break;
			default:
				break;
			}
		}
	}
	p->attr[cc]=pattr[cc];
	if (cc<2) p->attr[++cc]= -1;
}

char *mfgets(char *buf, int size, FILE *fp)
{
	int c, len;

	if ((len = input_line2(fp, (unsigned char *) buf, 0, size, &c)) == 0
		&& c != '\r' && c != '\n') return NULL;
	if (c == '\n' || c == '\r') {
		if (len+1 < size) strcat(buf+len, "\n");
		else ungetc(c, fp);
	}
	if (c == EOF) return NULL;
	return buf;
}

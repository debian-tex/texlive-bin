/* Written by K.Asayayam  Sep. 1995 */
#ifdef KPATHSEA

#include <kpathsea/kpathsea.h>
#include <string.h>
#include "kp.h"

int
KP_init(char *prog)
{
  kpse_set_program_name(prog, NULL);
  return 0;
}

/* KP_get_value(char *var,char *def_val)
     ARGUMENTS:
       char *var:     name of variable.
       char *def_val: default value.
 */
const char *KP_get_value(const char *var, const char *def_val)
{
  const char *p = kpse_var_value(var);
  return p ? p : def_val;
}

/* KP_get_path(char *var, char *def_val)
     ARGUMENTS:
       char *var:     name of variable.
       char *def_val: default value.
 */
static const char *KP_get_path(const char *var, const char *def_val)
{
  char avar[50];
  const char *p;
  strcpy(avar, "${");
  strcat(avar, var);
  strcat(avar, "}");
  p = kpse_path_expand(avar);
  return (p && *p) ? p : def_val;
}

/*
 */
int KP_entry_filetype(KpathseaSupportInfo *info)
{
  info->path = KP_get_path(info->var_name,info->path);
  return 0;
}

/* KP_find_file(KpathseaSupportInfo *info, char *name)
     ARGUMENTS:
       KpathseaSupportInfo *info: Informations about the type of files.
       char *name:                Name of file.
 */
const char *KP_find_file(KpathseaSupportInfo *info, const char *name)
{
  char *ret;
  ret = kpse_path_search(info->path,name,1);
  if (!ret && info->suffix && !find_suffix(name)) {
    char *suff_name;
    suff_name = concat3(name,".",info->suffix);
    ret = kpse_path_search(info->path,suff_name,1);
    free(suff_name);
  }
  return ret ? ret : name;
}

#else
int KP_dummy_variable;
#endif

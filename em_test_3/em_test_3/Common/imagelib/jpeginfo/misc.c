/* misc.c - misc routines for jpeginfo
 * $Id$
 *
 * Copyright (c) Timo Kokkonen, 1997.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __APPLE__
    #include <unistd.h>
#endif
#include "jpeginfo.h"

static char *rcsid = "$Id$";

#if !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

int is_dir(FILE *fp)
{
 struct stat buf;
 
 if (!fp) return 0;
#ifdef DEF_WP8
 if (fstat(_fileno(fp),&buf)) return 0;
#else
 if (fstat(fileno(fp), &buf)) return 0;
#endif
 if (S_ISDIR(buf.st_mode)) return 1;
 return 0;
}


long filesize(FILE *fp) 
{
  struct stat buf;

  if (!fp) return -1;
#ifdef DEF_WP8
  if (fstat(_fileno(fp), &buf)) return -1;
#else
  if (fstat(fileno(fp), &buf)) return -1;
#endif
  return buf.st_size;
}


void delete_file(char *name, int verbose_mode, int quiet_mode)
{
  if (rcsid); /* so the compiler cannot optimize our rcsid string :) */

  if (!name) return;
  if (verbose_mode && !quiet_mode) fprintf(stderr,"deleting: %s\n",name);
#ifdef DEF_WP8
  if (_unlink(name) && !quiet_mode)
#else
  if (unlink(name) && !quiet_mode)
#endif
  fprintf(stderr, "Error unlinking file: %s\n", name);
}


char *fgetstr(char *s,int n,FILE *stream) 
{
  char *p;
  
  if (!stream || !s || n < 1) return NULL;
  if (!fgets(s,n,stream)) return NULL;
  p=&s[strlen(s)-1];
  while ((p>=s)&&((*p==10)||(*p==13))) *p--=0;
  return s;
}



char *md2str(unsigned char *digest, char *s)
{
  int i;
  char buf[16],*r;

  if (!digest) return NULL;
  if (!s) {
    s=(char*)malloc(33);
    if (!s) return NULL;
  }

  r=s;
  for (i = 0; i < 16; i++) {
    sprintf (buf,"%02x", digest[i]);
    *(s++)=buf[0];
    *(s++)=buf[1];
  }
  *s=0;

  return r;
}

/* eof :-) */







#ifndef WRAPPERS_H
#define WRAPPERS_H

void *Malloc(size_t nbytes);

FILE *Fopen (char *fname, char *mode);

void myFclose (FILE *fp, char *fname);

size_t Fwrite (const void *buf, size_t size, size_t count, FILE *fp);

#endif

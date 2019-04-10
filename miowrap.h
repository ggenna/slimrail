#ifndef MIOWRAP_H
#define MIOWRAP_H

#include <stdarg.h>
#include <unistd.h>


extern int daemon_info_proc;


int Readline_file (int fd, void *ptr, size_t maxlen);
int readline_file (int fd, void *vptr, size_t maxlen); 
int readline_aut (int fd, void *vptr, size_t maxlen,long timeoutmio) ;
int Readlinetime (int fd, void *ptr, size_t maxlen,long timeout);
void Writen (int fd, void *ptr, size_t nbytes);
int Open(const char *pathname,int flags);

#endif


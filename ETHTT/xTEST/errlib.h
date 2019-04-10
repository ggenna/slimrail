/*

module: errlib.h

purpose: definitions of function sin errlib.c

reference: Stevens, Unix network programming (2ed), p.922

*/

#include <stdarg.h>

extern int daemon_proc;

void err_msg (const char *fmt, ...);

void err_quit (const char *fmt, ...);

void err_ret (const char *fmt, ...);

void err_sys (const char *fmt, ...);

void err_dump (const char *fmt, ...);

#include <stdlib.h> // getenv()
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h> // timeval
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h> // inet_aton()
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

#include "errlib.h"
#include "sockwrap.h"

#define INTERRUPTED_BY_SIGNAL (errno == EINTR || errno == ECHILD)

#define NOBLOCK (errno != EAGAIN)


extern char *prog;

int Socket (int family, int type, int protocol)
{
  int n;
  if ( (n = socket(family,type,protocol)) < 0)
    err_sys ("(%s) error - socket() failed", prog);
  return n;
}

void Bind(int sockfd,  const SA *myaddr,  socklen_t myaddrlen)
{
  if ( bind(sockfd, myaddr, myaddrlen) != 0)
    err_sys ("(%s) error - bind() failed", prog);
}


ssize_t writen (int fd, const void *vptr, size_t n)
{
  size_t nleft;
  ssize_t nwritten;
  const char *ptr;

  ptr = vptr;
  nleft = n;
  while (nleft > 0)
  {
    if ( (nwritten = write(fd, ptr, nleft)) <= 0)
    {
      if (INTERRUPTED_BY_SIGNAL)
      {
      	nwritten = 0;
        continue; /* and call write() again */
      }
      else
      	return -1;
    }
    nleft -= nwritten;
    ptr   += nwritten;
  }
  return n;
}



/* reads exactly "n" bytes from a descriptor */

int readn (int fd, void *vptr, size_t n)
{
  size_t nleft;
  size_t nread;
  char *ptr;

  ptr = vptr;
  nleft = n;
  while (nleft > 0)
  {
    if ( (nread = read(fd, ptr, nleft)) < 0)
    {
      if (INTERRUPTED_BY_SIGNAL)
      {
        nread = 0;
        continue; /* and call read() again */
      }
      else
        return -1;
    }
    else
      if (nread == 0)
        break; /* EOF */

    nleft -= nread;
    ptr   += nread;
  }
  return n - nleft;
}

int Select (int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout)
{
  int n;
again:
  if ( (n = select (maxfdp1, readset, writeset, exceptset, timeout)) < 0)
  {
    if (INTERRUPTED_BY_SIGNAL)
      goto again;
    else
      err_sys ("(%s) error - select() failed", prog);
  }
  return n;
}

ssize_t Recvfrom (int fd, void *bufptr, size_t nbytes, int flags, SA *sa, socklen_t *salenptr)
{
  ssize_t n;

  if ( (n = recvfrom(fd,bufptr,nbytes,flags,sa,salenptr)) < 0)
  	{
  	if (NOBLOCK)
    	err_sys ("(%s) error - recvfrom() failed", prog);
    else
       err_msg ("EAGAIN error - recvfrom() failed");	
    }
    else
    	err_msg ("OK - recvfrom()");	
  return n;
}


void Sendto (int fd, void *bufptr, size_t nbytes, int flags, const SA *sa, socklen_t salen)
{
  if (sendto(fd,bufptr,nbytes,flags,sa,salen) != nbytes)
  	{
  
  	if (NOBLOCK)
   			 err_sys ("(%s) error - sendto() failed", prog);
   	else
   			 err_msg ("EAGAIN error - sendto() failed");
  }
  else
  	 err_msg ("OK - sendto()");
}

void Close (int fd)
{
	if(close(fd)!=0)
	   err_sys ("(%s) error - close() failed", prog);
}
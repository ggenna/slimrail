#include <stdio.h>
#include <stdlib.h> /* exit() */
#include <stdarg.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>


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


#include <fcntl.h>


#include "errlib.h"

#include "miowrap.h"
#include "sockwrap.h"

//#define TIMEOUT 10
#define MAXLINE 4095


extern char *prog;
//extern long TIMEOUT;


int readline_file (int fd, void *vptr, size_t maxlen) 
{

  int n, rc;
  char c, *ptr;

  ptr = vptr;
  for (n=1; n<maxlen; n++)
  {
    if ( (rc = readn(fd,&c,1)) == 1)
    {
            *ptr++ = c;
      if (c == '\n')
      	break;	/* newline is stored, like fgets() */


    }
    else if (rc == 0)
    {
      if (n == 1)
      	return 0; /* EOF, no data read */
      else
      	break; /* EOF, some data was read */
    }
    else
      return -1; /* error, errno set by read() */
  }
  *ptr = 0; /* null terminate like fgets() */  

  return n;
}

/*La funzione legge fino a incontrare \r o \n 
 * ritorna n byte letti
 * 	   0 se riceve EOF
 * 	   err_sys= in caso di errore	
 */

int Readline_file (int fd, void *ptr, size_t maxlen)
{
  int n;

  if ( (n = readline_file(fd, ptr, maxlen)) < 0)
     	  err_sys ("(%s) error - readline_file() failed", prog);

  return n;
}



int readline_aut (int fd, void *vptr, size_t maxlen,long timeoutmio) 
{
 int n, rc;
  char c, *ptr=0;
  int ret,maxfdp1;
  fd_set readset;
  struct timeval timeout;
	FD_ZERO (&readset);
	FD_SET (fd, &readset);
	timeout.tv_sec=timeoutmio; 
	timeout.tv_usec=0;
       	maxfdp1 = fd + 1;


while((ret=Select(maxfdp1, &readset, NULL, NULL,&timeout))>0)
{

  ptr = vptr;
  for (n=1; n<maxlen; n++)
  {	  
	
		    if ( (rc =readn(fd,&c,1)) == 1 )
		    {
		       *ptr++ = c;

		    if (c == '\n')
		    {
                    // err_msg ("E' arrivato NF");
		      	break;	/* newline is stored, like fgets() */
		      }


/*
			if((ret=Select(maxfdp1, &readset, NULL, NULL,&timeout))==0)    		
				return -3;
*/

		    }
		    else if (rc == 0)
		    {
		      if (n == 1)
		      	return 0; /* EOF, no data read */
		      else
		      	break; /* EOF, some data was read */
		    }
		    else
		      return -1; /* error, errno set by read() */


     
 

  }
  *ptr = 0; /* null terminate like fgets() */

  return n;

}


return -3;// tmeout fired
  
}
/*
 * la Readlinetime legge dal socket fino a incontrare la sequenza di byte \r\n oppure maxlen byte
 * ritorna:	n= byte letti
 * 		0= EOF
 * 	       -3= scatta il timeout
 * 	  err_sys= caso fallimento
 **/
int Readlinetime (int fd, void *ptr, size_t maxlen,long timeout)
{
  int n;

  if ( (n = readline_aut(fd, ptr, maxlen,timeout)) < 0)
  {
    if(n==-3)
    err_msg ("readline_aut() timeout=%d sec fired", timeout);
    
     if(n==0)
    err_msg ("readline_aut() EOF, no data read %s",prog);
    
     if(n==-1)
    err_msg ("readline_aut()  error, errno set by read() %s",prog);
    
  }

  return n;
}

void Writen (int fd, void *ptr, size_t nbytes)
{
  if (writen(fd, ptr, nbytes) != nbytes)
    err_msg ("(%s) error - writen() failed", prog);
}

int Open(const char *pathname,int flags)
{
	int ret;
	
	if((ret=open(pathname,flags))== -1)
		 err_sys ("(%s) error - open() failed", prog);
	
	return ret;
}
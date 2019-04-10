#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>



#include <stdarg.h>


#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#ifdef __GNUC__ /* ---- */
#include <unistd.h>
#include <termios.h>
#define __USE_GNU
#include <pthread.h>
#undef __USE_GNU
#include <sys/time.h>
#else /* -------------- */
#include <Windows.h>
#include <io.h>
#include <time.h>
#include <sys/timeb.h>
#endif /* ------------- */

#include <sys/ioctl.h>
#include <linux/serial.h>
#include <sys/stat.h>
#include "errlib.h"
#include "apiMio.h"



/*
inseriti in piu da giovanni

*/
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/select.h>



char *prog;



/*****************************************
CHECK DEVICE 
******************************************/

int ischaracterdevice(const char *filename)
{
     struct stat status;
     mode_t mode;

     if (stat(filename, &status) == -1) {
  err_msg ("slimport: ischaracterdevice()");
	  return 0;
     };
     mode = status.st_mode;
     if (!S_ISCHR(mode)) {
		 err_msg ("ERROR: %s is not a character device.\n",prog);
	  return 0;
     }
     return 1;
}     


int isserialdevice(const char *devicename,int fd)
{
     struct serial_struct serinfo;

     if (!isatty(fd)) {
	    err_msg ("(%s) open()=%s", prog,devicename);
	  return 0;
     }
     serinfo.reserved_char[0] = 0;
     if (ioctl(fd, TIOCGSERIAL, &serinfo) < 0) {
           err_msg ("(%s)ERROR: %s is not a serial device.\n", prog,devicename);
	  return 0;
     }
     return 1;    
}


/**************************************
START COORDINATOR
**************************************/



int init_serial(xbee_hnd *xbee, int baudrate) {

  speed_t chosenbaud;
 int flags=0;


  /* select the baud rate */
  switch (baudrate) {
  case 1200:  chosenbaud = B1200;   break;
  case 2400:  chosenbaud = B2400;   break;
  case 4800:  chosenbaud = B4800;   break;
  case 9600:  chosenbaud = B9600;   break;
  case 19200: chosenbaud = B19200;  break;
  case 38400: chosenbaud = B38400;  break;
  case 57600: chosenbaud = B57600;  break;
  case 115200:chosenbaud = B115200; break;
  default:
    fprintf(stderr,"%s(): Unknown or incompatiable baud rate specified... (%d)\n",__FUNCTION__,baudrate);
    return -1;
  };

err_msg ("il path e' %s\n", xbee->path);

  if (!ischaracterdevice(xbee->path)) {
	  err_msg ("(%s) open()", prog);

     }


  /* open the serial port as a file descriptor */
  if ((xbee->ttyfd = open(xbee->path,O_RDWR | O_NOCTTY | O_NONBLOCK)) == -1) {
//   
/* */
    err_msg ("(%s) xbee_setup():open()", prog);

    return -1;
  }
  

if ( (flags=fcntl(xbee->ttyfd,F_GETFL,0)) <0 )
{
 err_msg ("(%s) F_GETFL", prog);
	return -1;
}
flags &= ~ O_NONBLOCK;
if ( fcntl(xbee->ttyfd,F_SETFL,flags) <0 )
{
err_msg ("(%s) F_SETFL", prog);
	return -1;
}
   
 

 err_msg ("il ttyfd 1 e' %d\n", xbee->ttyfd);

  bzero(&(xbee->status), sizeof((xbee->status))); /* clear struct for new port setting */
 


//  ioctl(xbee->ttyfd,TIOCMGET,&(xbee->status)); // non so che fa
//  fcntl(xbee->ttyfd, F_SETFL, 0);  // non so che fa

  if (!isserialdevice(xbee->path,xbee->ttyfd)) {
	      err_msg ("(%s) open()", prog);
     return -1;
     }




  /* check fd  */

     if (tcgetattr(xbee->ttyfd, &(xbee->status)) == -1) {
	 /* perror("openserial()");
	  closeserial();
	  erroropenserial(devicename);*/
return -1;
     }   




/*
 * scrivo la stringa di 125 byte
 * ricevo la stringa di 125 byte fino a LF
 */


  /* input flags */
  (xbee->status).c_iflag &= ~ IGNBRK;           /* enable ignoring break */
  (xbee->status).c_iflag &= ~(IGNPAR | PARMRK); /* disable parity checks */
  (xbee->status).c_iflag &= ~ INPCK;            /* disable parity checking */
  (xbee->status).c_iflag &= ~ ISTRIP;           /* disable stripping 8th bit */
  (xbee->status).c_iflag |= ICRNL;   /*     enable translating CR -> NL  in input, in output scrivo CR*/
  (xbee->status).c_iflag &= ~(IGNCR | INLCR);/* disable ignoring CR,disable translating NL -> CR */
  (xbee->status).c_iflag &= ~(IXON | IXOFF);    /* disable XON/XOFF flow control */

  /*output flag*/
  (xbee->status).c_oflag &= ~ OPOST;            /* disable output processing */
  (xbee->status).c_oflag &= ~(ONLCR | OCRNL);   /* disable translating NL <-> CR */
  (xbee->status).c_oflag &= ~ OFILL;            /* disable fill characters */
 

/*CONTROL FLAG*/
  (xbee->status).c_cflag |= (CLOCAL | CREAD);   /* Enable the receiver and set local mode */
  /* no parity (8N1)*/
  (xbee->status).c_cflag &= ~ PARENB;           /* disable parity */
  (xbee->status).c_cflag &= ~ CSTOPB;           /* disable 2 stop bits */
  (xbee->status).c_cflag &= ~ CSIZE;            /* remove size flag... */
  (xbee->status).c_cflag |=   CS8;              /* ...enable 8 bit characters */

  (xbee->status).c_cflag &= ~   HUPCL;            /* vedi |= enable lower control lines on close - hang up */

/* local flags */
  (xbee->status).c_lflag &= ~ ISIG;             /* disable generating signals */
  (xbee->status).c_lflag &= ~ ICANON;           /* disable canonical mode - line by line */
  (xbee->status).c_lflag &= ~ ECHO;             /* disable echoing characters */
  (xbee->status).c_lflag &= ~ ECHONL;           /* disable echo NL */
  (xbee->status).c_lflag &= ~ ECHOE;		/* disable echo char as BS-SP-BS */
  (xbee->status).c_lflag &= ~ NOFLSH;           /* disable flushing on SIGINT */
  (xbee->status).c_lflag &= ~ IEXTEN;           /* disable input processing */

 	 /*set speed*/
	cfsetispeed(&(xbee->status),chosenbaud);
	cfsetospeed(&(xbee->status),chosenbaud);


	(xbee->status).c_cc[VMIN]  = 5; /*17  almeno 80 caratteri, specifies the minimum number of characters to buffer */
	(xbee->status).c_cc[VTIME] = 1000; /* 100 timeout di 500ms ( 5*0,1 s), the maximum amount of inter-character time
                                         *  (in tenths of a second) before the device driver returns the buffered input 
                                         *  to a process reading from the serial port
                                         */

	/*
	 * Set the new options for the port...
	*/
	if(tcflush(xbee->ttyfd, TCIOFLUSH)==-1)
	{
		   err_msg ("(%s) tcflush()", prog);
		return -1;
	}	
	if(tcsetattr(xbee->ttyfd, TCSANOW, &(xbee->status))==-1) 
	{
		   err_msg ("(%s) tcsetattr()", prog);
		return -1;
	}
 
return 0;

}























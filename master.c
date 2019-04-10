#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/select.h>

#define _POSIX_SOURCE 1         //POSIX compliant source
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;


int wait_flag=TRUE;                     //TRUE while no signal received
char devicename[80];
int status;

int send(int fd, int count)   /* invia dati al modem */
{
	int n;
	char buffer[255];  /* Input buffer */
  	char *bufptr;      /* Current char in buffer */
	char message[90];

	printf("trasmetto: %03d\n", count);
	sprintf(message, "PING%03d", count);

	write(fd, message, 7);

         printf("trasmetto: %d\n", fd);
	bufptr = buffer;

	while ((n = read(fd, bufptr, buffer + sizeof(buffer) - bufptr - 1)) > 0)
	{
		bufptr += n;
		/*if (bufptr[-1] == '\n' || bufptr[-1] == '\r')
			break;*/
		if ((bufptr - buffer) > 6)
			break;

	}

	/* nul terminate the string and see if we got an OK response */

	*bufptr = '\0';
	printf("ricevo: %s\n", buffer);

	return n;

}

main(int argc, char *argv[])
{

	char Param_strings[7][80];
	char message[90];

	int fd, i, error;
	struct termios oldtio, newtio;       //place for old and new port settings for serial port
	char buf[255];                       //buffer for where data is put
	int n = 0;
	int recv = 0;

	error=0;

	//read the parameters from the command line
	if (argc==2)  //if there are the right number of parameters on the command line
	{
		for (i=1; i<argc; i++)  // for all wild search parameters
		{
			strcpy(Param_strings[i-1],argv[i]);
 		}

		i=sscanf(Param_strings[0],"%s",devicename);
		if (i != 1) error=1;

		sprintf(message,"Device=%s, Baud=19200\n",devicename); //output the received setup parameters
		puts(message);

	} else {
		fprintf(stderr, "uso: master port\n");
		exit(1);
	}
       
	puts("---");

	//open the device(com port) to be non-blocking (read will return immediately)
	fd = open(devicename,  O_RDWR | O_NOCTTY  | O_NDELAY);

	if (fd < 0)
	{
         	perror(devicename);
         	exit(-1);
      	}

	fcntl(fd, F_SETFL, 0);

	tcgetattr(fd,&oldtio); /* save current port settings */

	bzero(&newtio, sizeof(newtio));
	cfsetispeed(&newtio, B19200);
	cfsetospeed(&newtio, B19200);

	/*
	 * Enable the receiver and set local mode...
	 */

	newtio.c_cflag |= (CLOCAL | CREAD);

	newtio.c_cflag &= ~CSIZE; /* Mask the character size bits */
	newtio.c_cflag |= CS8;    /* Select 8 data bits */

	newtio.c_cflag &= ~PARENB;
	newtio.c_cflag &= ~CSTOPB;


	newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	newtio.c_oflag &= ~OPOST;

	newtio.c_cc[VMIN]  = 7;
	newtio.c_cc[VTIME] = 100;

	/*
	 * Set the new options for the port...
	 */
	
	tcsetattr(fd, TCSANOW, &newtio);
	tcflush(fd, TCIOFLUSH);

	/* programma principale */
	for (n = 0; n < 255; n++)
	{
		send(fd, n);
	}
	
	// restore old port settings
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);        //close the com port
}  //end of main

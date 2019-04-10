#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <termios.h> /* Terminal control library (POSIX) */
#define MAX 100
main(int argc, char *argv[])
{
	int sd=3;
	char *serialPort="";
	char *serialPort0="/dev/ttyS0";
	char *serialPort1="/dev/ttyS1";
	char *USBserialPort0="/dev/ttyUSB0";
 char *USBserialPort1="/dev/ttyUSBS1";
 char valor[MAX]="";
 char c;
 char *val;
 struct termios opciones;
 int num;
 char *s0="S0";
 char *s1="S1";
 char *u0="USB0";
 char *u1="USB1";

 if(argc!=2)
 {
 fprintf(stderr,"Usage: %s [port]\nValid ports: (S0, S1, USB0, USB1)\n",argv[0], serialPort);
 exit(0);
 }
 if(!strcmp(argv[1],s0))
 {
 fprintf(stderr,"ttyS0 chosen\n...");
 serialPort=serialPort0;
 }
 if(!strcmp(argv[1],s1))
 {
 fprintf(stderr,"ttyS1 chosen\n...");
 serialPort=serialPort1;
 }
 if(!strcmp(argv[1],u0))
 {
 fprintf(stderr,"ttyUSB0 chosen\n...");
 serialPort=USBserialPort0;
 }
 if(!strcmp(argv[1],u1))
 {
 fprintf(stderr,"ttyUSB1 chosen\n...");
 serialPort=USBserialPort1;
 }
 if (!strcmp(serialPort,""))
 {
 fprintf(stderr,"Choose a valid port (S0, S1, USB0, USB1)\n", serialPort);
 exit(0);
 }

if ((sd = open(serialPort, O_RDWR | O_NOCTTY | O_NDELAY)) == -1)
 {
 fprintf(stderr,"Unable to open the serial port %s - \n", serialPort);
 exit(-1);
 }
 else
 {
 if (!sd)
 {
 sd = open(serialPort, O_RDWR | O_NOCTTY | O_NDELAY);
 }
 //fprintf(stderr,"Serial Port open at: %i\n", sd);
 fcntl(sd, F_SETFL, 0);
 }
 tcgetattr(sd, &opciones);
 cfsetispeed(&opciones, B19200);
 cfsetospeed(&opciones, B19200);
 opciones.c_cflag |= (CLOCAL | CREAD);
 /*No parity*/

opciones.c_cflag &= ~PARENB;
 opciones.c_cflag &= ~CSTOPB;
 opciones.c_cflag &= ~CSIZE;
 opciones.c_cflag |= CS8;
 /*raw input:
 * making the applycation ready to receive*/
 opciones.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
 /*Ignore parity errors*/
 opciones.c_iflag |= ~(INPCK | ISTRIP | PARMRK);
 opciones.c_iflag |= IGNPAR;
 opciones.c_iflag &= ~(IXON | IXOFF | IXANY | IGNCR | IGNBRK);
 opciones.c_iflag |= BRKINT;
 /*raw output
 * making the applycation ready to transmit*/
 opciones.c_oflag &= ~OPOST;
 /*aply*/
 tcsetattr(sd, TCSANOW, &opciones);
 int j=0;
 while(1)
 {
  while(read(sd,&c,1));
 valor[j]=c;
 j++;
 //We start filling the string until the end of line char arrives

 if((c=='\r') || (j==(MAX-1)))
 {
 int x;
 for(x=0;x<j;x++)
 {
 write(2,&valor[x],1);
 valor[x]='\0';
 }
 j=0;
 }
 }
 close(sd);
}

 //or we reach the end of the string. Then we write it on the screen.

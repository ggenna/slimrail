#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>  


#include <sys/time.h>

#define ifreq_offsetof(x)  offsetof(struct ifreq, x)


#include <arpa/inet.h>

#include <time.h> // needed to know the current date and time

#include "errlib.h"
#include "sockwrap.h"

#define MAXBUFL 70 
#define DAYTIME_PORT 5005

char *prog;


unsigned int setTimer(unsigned int tempo)
{
	unsigned int msec;
	
	struct timeval start;
	gettimeofday (&start, NULL);
	msec= (start.tv_sec * 1000) + (start.tv_usec / 1000);
	
	msec=msec+tempo;
	return msec;
}


unsigned int gettimestamp(time_t *data,suseconds_t *time)
{
	unsigned int ret=0;
	
	struct timeval start;
	gettimeofday (&start, NULL);
	
	*data= start.tv_sec;
	*time=start.tv_usec;
	return ret;
}


void stampatimestamp(time_t timestamp,suseconds_t time)
{
	struct timeval tv;
	struct tm* ptm;
	char time_string[40];
	suseconds_t milliseconds=0;
	
	
	
	tv.tv_sec=(timestamp);

	ptm = localtime (&tv.tv_sec);
	/* Format the date and time, down to a single second. */
	strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
	/* Compute milliseconds from microseconds. */
	milliseconds = (time/1000);
	/* Print the formatted time, in seconds, followed by a decimal point
	 and the milliseconds. */
	err_msg ("%s.%03ld\n", time_string, milliseconds);
	
	
}


int main (int argc, char *argv[])
{
	int sockfd, n;
	unsigned int count=0,nodo=0,carro=0;
  char buf[MAXBUFL];
   char input[MAXBUFL];
	char temp[3]={'\0','\0','\0'};

uint8_t versione=0,diagnostica=0;	
uint16_t idfabb=0;
uint32_t numserie=0;
time_t datainizio=0;

  struct sockaddr_in servaddr;
	
	unsigned int timestamp2=0,timestamp3=0;
	time_t data;
	suseconds_t time=0; // milliseconds
		 int on=1;

  /* for errlib to know the program name */

  prog = argv[0];
	
	sscanf(argv[2],"%d",&count);
  /* check the arguments */
	
	count=(unsigned char)count;
	
	
  if (argc != 3)
    err_quit ("usage: %s IPaddress_of_daytime_server ", prog);
  /* create socket */

  sockfd = Socket (AF_INET, SOCK_DGRAM, 0);
   setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));	 
	
  /* specify address of server to speak to */

  memset (&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port   = htons(DAYTIME_PORT);
  Inet_aton(argv[1], &servaddr.sin_addr);

  /* send the request and read the answer */
	
	timestamp2=setTimer(0);
	//timestamp=setTimer(100);
	gettimestamp(&datainizio,&time);
	
	while(1){
	
	
	
	printf("ciao\n");
		
	timestamp3=setTimer(0);	
		//timestamp=	gettimestamp();
		
		
		
	if(timestamp2<timestamp3)
	{
	

	
	timestamp2=setTimer(100);
		
	
		
    memset (buf, 0, sizeof(buf));		
		
	sprintf(buf,"%d",count);	
		
    memcpy(buf+strlen(buf),"--",3-strlen(buf));	
		
	memcpy(buf+3,"-",1);	
	
	memcpy(buf+3+1,"GATEWAY-U",9);
	
	
	memset (input,0, sizeof(input));
	err_msg("numero carro [max 2 car]:\t");
	Readline(0,input,MAXBUFL);
	
        memset (temp,0, sizeof(temp));
	memcpy(temp,input,strlen(input)-1);	

	//	sprintf(temp,"%d",carro);	
		memcpy(buf+1+3+9,"00",2);	
		memcpy(buf+1+3+9+2-strlen(temp),temp,strlen(temp));	

		
		memcpy(buf+3+1+9+2,"-",1);
		
		if(carro<98)
			carro++;
		else 
			carro=0;
		
	
	
	memset (temp,0, sizeof(temp));	
	//sprintf(temp,"%d",nodo);
	memset (input,0, sizeof(input));
	err_msg("numero nodo [max 2 car]:");
	Readline(0,input,MAXBUFL);	
	memcpy(temp,input,strlen(input)-1);	
	
		
	memcpy(buf+1+3+12,"00",2);	
	memcpy(buf+1+3+12+2-strlen(temp),temp,strlen(temp));		
	
	memcpy(buf+3+1+12+2,"-",1);
	
	if(nodo<7)
		nodo++;
	else 
		nodo=0;
	

		
	memcpy(buf+3+1+15,&timestamp3,4);
	
	
	memset (input,0, sizeof(input));
	err_msg("Classe dispositivo [1 car]:");
	Readline(0,input,MAXBUFL);	
	
	memcpy(buf+3+1+15+4,"---",3);			
	memcpy(buf+3+1+15+4+1,input,1);	




	
	memset (input,0, sizeof(input));
	err_msg("versione e revisione [1 byte]:");
	Readline(0,input,MAXBUFL);		
	sscanf(input,"%d",&versione);
	memcpy(buf+3+1+15+4+3,&versione,1);		
	
	memset (input,0, sizeof(input));
	err_msg("diagnostica [1 byte]:");
	Readline(0,input,MAXBUFL);		
	sscanf(input,"%d",&diagnostica);
	memcpy(buf+3+1+15+4+3+1,&diagnostica,1);
	
	memset (input,0, sizeof(input));
	err_msg("idfabb [2 byte]:");
	Readline(0,input,MAXBUFL);		
	sscanf(input,"%u",&idfabb);
	memcpy(buf+3+1+15+4+3+1,&idfabb,2);		
	
	memset (input,0, sizeof(input));
	err_msg("numserie [4 byte]:");
	Readline(0,input,MAXBUFL);		
	sscanf(input,"%u",&numserie);
	memcpy(buf+3+1+15+4+3+1+2,&numserie,4);		
	
	
	
	memcpy(buf+3+1+15+4+3+1+1+2+4,&datainizio,4);	
	
	
	
	
	gettimestamp(&data,&time);
		
	memcpy(buf+3+1+15+4+3+12,&data,4);		
		//28
	memcpy(buf+3+1+15+4+3+12+4,"NET-SONO-MOLTO-SI-CHE-FUNGO\n",28);			
		
  Sendto (sockfd, buf, MAXBUFL, 0, (SA*) &servaddr, sizeof(servaddr));
   
	//	err_msg ("buf:%s", buf);
		//err_msg ("buf:%d %d %d %d", buf[18], buf[19], buf[20], buf[21]);
	//	err_msg ("timestamp:%ld", timestamp);
//		err_msg ("timestamp:%d %d %d %d", buf[19],buf[20],buf[21],buf[22]);
		
		stampatimestamp(data,time);
	//	err_msg ("timestamp:%ld",timestamp3);
		
	//	err_msg ("dim buf:%d",strlen(buf));
		
		
		

 // n = Recvfrom (sockfd, buf, MAXBUFL, 0, NULL, NULL);
		
		
	}
		if(count>0)
			count--;
		else 
			count=255;

}
  /* display the answer and quit */

//  buf[n++] = '\n';
//  Write (1, buf, n);

  return 0;
}

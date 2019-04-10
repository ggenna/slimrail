#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>             /* offsetof */
#include <net/if.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <asm/types.h>
#include <linux/if_ether.h>
#include	<sys/ioctl.h>	
#include	<errno.h>

#include <sys/time.h>

#define ifreq_offsetof(x)  offsetof(struct ifreq, x)





#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <time.h> // needed to know the current date and time

#include "errlib.h"
#include "sockwrap.h"
#include "miowrap.h"
#include "interfacciaeth.h"

#define GW_PORT 5005
#define START 1
#define STOP 0
#define RESTART 2
#define DIMPACK 70

#define IFTRENO "eth0"
#define IFTERRA "eth2"



//#include <sys/param.h>
//#include <sys/system.h>

uint32_t   crc32_tab[] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de,	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,	0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940,	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,	0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};


char *prog;


uint32_t crc32(const void *buf, unsigned char size)
{
	const unsigned char *p;
uint32_t   crc;

	p = buf;
	crc = ~0U;

	while (size--)
		crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);

	return crc ^ ~0U;
}




/* inizialize()
 * void
 *
 */
 
eth_nodo *inizializeETH(void)
{
		   /* create a new instance */

eth_nodo *gw=NULL;	   
		   
  gw =(eth_nodo *)calloc(1,sizeof(eth_nodo));
  
	  memset((gw->sendBufferTreno),0,sizeof(gw->sendBufferTreno));
	  memset((gw->receiveBufferTreno),0,sizeof(gw->receiveBufferTreno));
	  memset((gw->sendBufferTerra),0,sizeof(gw->sendBufferTerra));
	  memset((gw->receiveBufferTerra),0,sizeof(gw->receiveBufferTerra));
	  memset((gw->nome),0,sizeof(gw->nome));
		
		memset(&(gw->servtreno),0,sizeof(gw->servtreno));
		memset(&(gw->servterra),0,sizeof(gw->servterra));
		memset(&(gw->broadcast),0,sizeof(gw->broadcast));		
		
		memset(&(gw->cliaddr),0,sizeof(gw->cliaddr));
		memset(&(gw->ifrtreno),0,sizeof(gw->ifrtreno));
		memset(&(gw->ifrterra),0,sizeof(gw->ifrterra));	
		gw->sockfdtreno=0;
		gw->sockfdterra=0;
		gw->stato=0;
		memset(&(gw->clilen),0,sizeof(socklen_t));
		memset(&(gw->broadlen),0,sizeof(socklen_t));

	  return gw;
}
 
 
/* set_no_blocking(int socketfd)
 * ret: -1 Errore
 *			 0	OK
 *
 */
int set_no_blocking(int socketfd)
{
	int flags,ret=-1;
	// SETTO IL SOCKET COME NON BLOCCANTE
	if ( (flags=fcntl(socketfd,F_GETFL,0)) <0 )
		err_msg("fcntl(F_GETFL) failed, Err: %s", prog);
	else
		{
			flags |= O_NONBLOCK; 
			if ( fcntl(socketfd,F_SETFL,flags) <0 )
				err_msg("fcntl(F_SETFL) failed, Err: %s", prog);
			else
				ret=0;
	   }
	   
	return ret;
}




/* init_serverUDP()
 *
 *
 *
 */
int init_serverUDP(eth_nodo *gw)
{
	int ret=-1;
	     char *p,*pterra;
	 int on=1;
	 struct sockaddr_in mem,memterra;
	 char IP_ETH0[13];
	 char IP_ETH1[13];
	 char IP_TX[13];



	/*crea indirizzo ip GATEWAY e BROADCAT*/
		  memset(&(mem),0,sizeof(mem));
		  memset(&(memterra),0,sizeof(memterra));
		  memcpy((IP_ETH1),"192.168.0.3",11);
      memcpy((IP_ETH0),"192.168.1.254",13);
      memcpy((IP_TX),"192.168.1.1",13);
     	
		
	/* create socket */
  gw->sockfdtreno = Socket(AF_INET, SOCK_DGRAM, 0);
  gw->sockfdterra = Socket(AF_INET, SOCK_DGRAM, 0);	
	/* set socket to sending  BROADCAST  MODE*/
	 setsockopt(gw->sockfdtreno, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
 	 setsockopt(gw->sockfdterra, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));	 
	 
   strncpy((gw->ifrtreno).ifr_name, IFTRENO, IFNAMSIZ);
   strncpy((gw->ifrterra).ifr_name, IFTERRA, IFNAMSIZ);
  
  
   /* indirizzo di ascolto  serv treno  */
	   (gw->servtreno).sin_family = AF_INET;
	  (gw->servtreno).sin_port = htons(GW_PORT);     
      mem=(gw->servtreno);
   (gw->servtreno).sin_addr.s_addr=htonl(INADDR_ANY);
//       (gw->servtreno).sin_addr.s_addr=inet_addr(IP_ETH1);
      inet_aton(IP_ETH1,&mem.sin_addr);
	  
	

	 
	  /* indirizzo di destinazione servterra    */    
     (gw->servterra).sin_family=AF_INET;
     (gw->servterra).sin_port=htons(GW_PORT);  //porta di lettura dei client differenziata 5001
     memterra=(gw->servterra);
    // (gw->servterra).sin_addr.s_addr=htonl(INADDR_ANY);
   (gw->servterra).sin_addr.s_addr=inet_addr(IP_ETH0);
     inet_aton(IP_ETH0,&memterra.sin_addr);
	 
	
	
	/* indirizzo broadcast di invio verso l'interfaccia eth0*/
	
    	(gw->broadcast).sin_family=AF_INET;
     (gw->broadcast).sin_port=htons(GW_PORT);  //porta di lettura dei client differenziata 5001
     (gw->broadcast).sin_addr.s_addr=inet_addr(IP_TX);
     inet_aton(IP_TX,&(gw->broadcast).sin_addr);
   
	
	
	  p = (char *)&mem;
	  memcpy( (((char *)&(gw->ifrtreno) + ifreq_offsetof(ifr_addr) )),p, sizeof(SA));
	  ioctl(gw->sockfdtreno,SIOCSIFADDR,&(gw->ifrtreno));
	  ioctl(gw->sockfdtreno, SIOCGIFFLAGS, &(gw->ifrtreno));
	  (gw->ifrtreno).ifr_flags |= IFF_UP | IFF_RUNNING;
	  ioctl(gw->sockfdtreno, SIOCSIFFLAGS, &(gw->ifrtreno));
	  //set_no_blocking(gw->sockfdtreno);    
    Bind (gw->sockfdtreno, (SA*) &(gw->servtreno), sizeof(gw->servtreno));
    
	err_msg("Bind indirizzo treno OK ");
	
	pterra = (char *)&memterra;
	  memcpy( (((char *)&(gw->ifrterra) + ifreq_offsetof(ifr_addr) )),pterra, sizeof(SA));
	  ioctl(gw->sockfdterra,SIOCSIFADDR,&(gw->ifrterra));
	  ioctl(gw->sockfdterra, SIOCGIFFLAGS, &(gw->ifrterra));
	  (gw->ifrterra).ifr_flags |= IFF_UP | IFF_RUNNING;
	  ioctl(gw->sockfdterra, SIOCSIFFLAGS, &(gw->ifrterra));
	  //set_no_blocking(gw->sockfdterra);    
  //  Bind(gw->sockfdterra, (SA*) &(gw->servterra), sizeof(gw->servterra));
   
		err_msg("Bind indirizzo terra OK ");
		
	  gw->stato=1; //  dichiaro aperto il socket
	  

	
	return ret;
	
	
}



/* serverUDP()
 *
 * ret=0 nessun campo letto
 * ret=1 buffer lettura pronto
 * DimWrite >0 dim messaggio da scrivere
 * DimWrite =0 nessun mess da scrivere
 */
int serverUDP(eth_nodo *gw,int DimWriteTreno,int DimWriteTerra)
{
	int ret=-1;
	int dimread=0;
	char tempBuffer[100];
	
	
	memset(tempBuffer,0,sizeof(tempBuffer));
	
	if(gw->stato==0)
			init_serverUDP(gw);
	
	if(gw->stato==1)
		{
					
					gw->clilen = sizeof(gw->cliaddr);
					
					memset((gw->receiveBufferTreno),0,sizeof(gw->receiveBufferTreno));
					
				
					dimread = Recvfrom(gw->sockfdtreno,tempBuffer, DIMPACK, 0, (SA*) &(gw->cliaddr), &(gw->clilen));
					 info_msg ("Arrivati:%d byte new request from client%s %u",dimread,inet_ntoa((gw->cliaddr).sin_addr), ntohs((gw->cliaddr).sin_port));

					
					if(dimread==DIMPACK)
						{
							// si devono fare ulteriori controlli nel pck per determinare se deve andare
							// verso la loco o verso la coda (check subnet destinazione e sorgente)
							memcpy(gw->receiveBufferTreno,tempBuffer,dimread);
						
								
							// mantengo il vecchio pak sull'input buffering
							ret=1;
						}
						else
							{
								ret=0;
								
							}
				
				/* si trtasmette solanto dall'interfaccia eth0 verso la rete di treno*/
				
						
				if(DimWriteTreno)
						{
			     	  	gw->broadlen = sizeof(gw->broadcast);
						   Sendto (gw->sockfdtreno, gw->sendBufferTreno, DimWriteTreno, 0, (SA*) &(gw->broadcast),gw->broadlen);			
			        // memset((gw->sendBufferTreno),0,sizeof(gw->sendBufferTreno));
			    }
			    
			    
			    
			    
			    if(DimWriteTerra)
			    	{
			     	  	gw->broadlen = sizeof(gw->broadcast);
						   Sendto (gw->sockfdterra, gw->sendBufferTerra, DimWriteTerra, 0, (SA*) &(gw->broadcast),gw->broadlen);			
			        // memset((gw->sendBufferTerra),0,sizeof(gw->sendBufferTerra));
			        	 info_msg ("Inviati:%d byte al server %s %u",DimWriteTerra,inet_ntoa((gw->broadcast).sin_addr), ntohs((gw->broadcast).sin_port));

			    }
			    
			  
			    
		}
	
	
	return ret;
	
	
}

/* find_host()
 *
 *
 *
 */
int find_host(eth_nodo *gw)
{
		int ret=-1;
		
		
		
		
		return ret;
}


int provalo(eth_nodo *gate)
{
		int ret=0;
		static unsigned char cont=8;
		char asciitmp[70];
		int numtemp=0;
		unsigned char header0=0,vagonaddr=0,idnode=0,modo=0,dev=0,payload1=0,payload2=0;// 1 byte
		unsigned int timestamp=0; // 32 bit
		unsigned short payload3=0; // 2 byte
		unsigned int payload4=0; // 4 byte
		unsigned int payload5=0; // 4 byte
		unsigned int payload6=0; // 4 byte
		
		time_t data=0;
uint32_t  CRC=0; // 4 byte
	 
	 
	 info_msg("gate->receiveBufferTreno%s",gate->receiveBufferTreno);
   if(strlen(gate->receiveBufferTreno))
   	  ret=70;
   
	// memset((gate->receiveBufferTerra),0,sizeof(gate->receiveBufferTerra));
	 memset((gate->sendBufferTerra),0,sizeof(gate->sendBufferTerra));
	  memset(asciitmp,0,sizeof(asciitmp));
	/*  

	 header0=((vagonaddr)<<3);
	 header0=header0|idnode;  // 1 byte
	 
	 info_msg("header0=%d",header0);	
	
	*/
	 
	 
	  memcpy(&modo,gate->receiveBufferTreno+12,1);
	   info_msg("modo=%c",modo);
	 
	  memcpy(asciitmp,gate->receiveBufferTreno+13,2);
	  sscanf(asciitmp,"%2d",&numtemp);
	  vagonaddr=(unsigned char)numtemp;
	  info_msg("carro=%02d",vagonaddr);
	  memset(asciitmp,0,sizeof(asciitmp));
   
    memcpy(asciitmp,gate->receiveBufferTreno+16,2);
    sscanf(asciitmp,"%2d",&numtemp);
    idnode=(unsigned char)numtemp;
	  info_msg("nodo=%02d",idnode);
	  memset(asciitmp,0,sizeof(asciitmp));
	 
	  memcpy(&timestamp,gate->receiveBufferTreno+19,4);
	//  info_msg("timestamp=%d %d %d",gate->receiveBufferTreno[19],gate->receiveBufferTreno[20],gate->receiveBufferTreno[21]);
	  info_msg("timestamp=%u",timestamp);
	  
    memcpy(&dev,gate->receiveBufferTreno+24,1);
    info_msg("dev=%c",dev);
    
	  memcpy(asciitmp,gate->receiveBufferTreno+26,12);
	   info_msg("payload0-11=%s",asciitmp);
	//  memset(asciitmp,0,sizeof(asciitmp));
	  memcpy(&data,gate->receiveBufferTreno+38,4);
	  stampatimestamp(data,0);
	  
	  
	 header0=((vagonaddr)<<3);
	 header0=header0|idnode;  // 1 byte
	 info_msg("header0=%d",header0);	
	 
   memcpy(gate->sendBufferTerra,&header0,1);
   info_msg("strlen=%d",strlen(gate->sendBufferTerra));
	 memcpy(gate->sendBufferTerra+1,&timestamp,4);
	   info_msg("strlen=%d",strlen(gate->sendBufferTerra));
	 memcpy(gate->sendBufferTerra+5,&modo,1);
	    info_msg("strlen=%d",strlen(gate->sendBufferTerra));
	 memcpy(gate->sendBufferTerra+6,&dev,1);
	    info_msg("strlen=%d",strlen(gate->sendBufferTerra));
   
    memcpy(gate->sendBufferTerra+7,asciitmp,12);
	    info_msg("strlen=%d",strlen(gate->sendBufferTerra));
   
    memcpy(gate->sendBufferTerra+19,&data,4);
	    info_msg("strlen=%d",strlen(gate->sendBufferTerra));
   
      
    CRC= crc32(gate->sendBufferTerra, sizeof(gate->sendBufferTerra)-sizeof(CRC));
    memcpy(gate->sendBufferTerra+66,&CRC,4);
    
        info_msg("massimo(uint32_t)=%u CRC=%u Dim Calcolo Frame senz CRC=%d",sizeof(uint32_t),CRC,sizeof(gate->receiveBufferTerra)-sizeof(CRC));
 
  
		return ret;
}



unsigned int setTimer(unsigned int tempo)
{
unsigned int msec;

  struct timeval start;
   gettimeofday (&start, NULL);
   msec= (start.tv_sec * 1000) + (start.tv_usec / 1000);
   
   msec=msec+tempo;
   return msec;
}


void stampatimestamp(time_t timestamp,suseconds_t time)
{
	struct timeval tv;
	struct tm* ptm;
	char time_string[40];
	suseconds_t milliseconds=0;
	
	
	
	tv.tv_sec=(timestamp);
	ptm = localtime (&tv.tv_sec);
	// Format the date and time, down to a single second. 
	strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
	// Compute milliseconds from microseconds. 
	milliseconds = time/1000;
	// Print the formatted time, in seconds, followed by a decimal point 	 and the milliseconds. 

	info_msg ("%s.%03ld\n", time_string, milliseconds);
	
	
}




/*
 int main(int argc, char *argv[]) {
	int i=0;
	
	eth_nodo *gateway=NULL;

gateway=inizializeETH();

  
  gateway->punt=gateway->receiveBuffer;
	memcpy(gateway->receiveBuffer,"CIAO",4);  

	
	err_msg ("buffer %s", gateway->receiveBuffer);
	err_msg ("punt %s", gateway->punt);
	provalo(gateway);
		err_msg ("buffer %s", gateway->punt);
	memcpy(gateway->receiveBuffer,"VAIC",4);
	err_msg ("buffer %s", gateway->punt);
 
  check_and_set_host(gateway,(unsigned char)2,(char)1);
  err_msg ("subnet %d", gateway->subnet);
  err_msg ("nomee %s", gateway->nome);
   err_msg ("modo %d", gateway->modo);
    check_and_set_host(gateway,(unsigned char)211,(char)2);
    err_msg ("modo %d", gateway->modo);
      err_msg ("nomee %s", gateway->nome);


while(i<259){	
	provalo(gateway);
	err_msg ("buffer %d byte:%s",strlen(gateway->punt), gateway->punt);
	i++;
}



//eth_nodo *gateway=NULL;
//unsigned char ncarro=3;
//
//gateway=inizializeETH();
//
////memcpy(gateway->nome,"PROVA_ETH",9);
//
//while(1)
//{
//serverUDP(gateway,0);
//}


return (0);
	
}

*/

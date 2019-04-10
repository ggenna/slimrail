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


#define ifreq_offsetof(x)  offsetof(struct ifreq, x)






#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <time.h> // needed to know the current date and time

#include "errlib.h"
#include "sockwrap.h"
#include "miowrap.h"
#include "GestioneETH.h"


#define GW_PORT 5005
#define START 1
#define STOP 0
#define RESTART 2
#define DIMPACK 70


char *prog;

/******************************************************************************************
 ******************************* GESTIONE GPIO ********************************************
 ******************************************************************************************
 */ 

/* int command_backupmode(int mode,int gpio)
 *
 * mode: GPIOOUT/GPIOIN
 * gpio: GPIO133,GPIO135
 * return ret=-1 ERRORE / ret=0 OK 
 * 	
 */
int command_backupmode(int mode,int gpio)
{
	int ret=-1;
	#ifdef ENABLEGPIO
 char comando[100];
 char strgpio[4];

int flag=0;

 memset(strgpio,0,sizeof(strgpio));  
  sprintf(strgpio,"%d",gpio);
 memset(comando,0,sizeof(comando));

  if(mode==BACKUPMODE)
  {
   memcpy(comando,"echo 1 > /sys/class/gpio/gpio",29);
   flag=1;
  }
  
  if(mode==NORMALMODE)
 { 
    memcpy(comando,"echo 0 > /sys/class/gpio/gpio",29);
    flag=1;
  }
  
  if(flag)
  {
  memcpy(comando+strlen(comando),strgpio,strlen(strgpio));
  memcpy(comando+strlen(comando),"/value",6);
    if(system(comando)>0)
       ret=0;
  err_msg ("Comando= \"%s\"",comando);
  }
  
  #endif
  return ret;
}

/* int initGPIO(int gpio,int tipo)
 *
 * tipo: BACKUPMODE/NORMALMODE
 * gpio: GPIO133,GPIO135
 * return ret=-1 ERRORE / ret=0 OK 
 * 	
 */
int initGPIO(int gpio,int tipo)
{
	 int ret=-1;
	#ifdef ENABLEGPIO
 char comando[100];
 char strgpio[4];


 
  memset(strgpio,0,sizeof(strgpio));
  
  sprintf(strgpio,"%d",gpio);

  memset(comando,0,sizeof(comando));
  memcpy(comando,"echo ",5);
  memcpy(comando+5,strgpio,strlen(strgpio));
  memcpy(comando+strlen(comando)," > /sys/class/gpio/export",25);
  if(system(comando)>0)
       ret=0;
  
  
  err_msg ("Comando= \"%s\"",comando);
  
  memset(comando,0,sizeof(comando));
  if(tipo==GPIOOUT)
  	 memcpy(comando,"echo out > /sys/class/gpio/gpio",31);
  if(tipo==GPIOIN)
  	 memcpy(comando,"echo in > /sys/class/gpio/gpio",30);
  
  
  memcpy(comando+strlen(comando),strgpio,strlen(strgpio));
  memcpy(comando+strlen(comando),"/direction",10);
  if(system(comando)>0)
       ret=0;
    
  err_msg ("Comando= \"%s\"",comando);
#endif
return ret;
}

/* int readGPIO(int gpio)
 *
 * gpio: GPIO133,GPIO135
 * return ret=-1 ERRORE / ret=0 OK / ret=1 OK
 * 	
 */


int readGPIO(int gpio)
{

	int gpiovalue=-1;
		#ifdef ENABLEGPIO
int fdfile=0;
	int ret=-1;
char buffile[5];
char strgpio[4];
char comando[50];


 memset(buffile,0,sizeof(buffile)); 
  memset(strgpio,0,sizeof(strgpio)); 
 memset(comando,0,sizeof(comando));  
 
  sprintf(strgpio,"%d",gpio);
    memcpy(comando,"/sys/class/gpio/gpio",20);
    memcpy(comando+strlen(comando),strgpio,strlen(strgpio));
    memcpy(comando+strlen(comando),"/value",6);

 fdfile=Open(comando,O_RDONLY);
 Readline_file(fdfile,buffile,5);
 ret=sscanf(buffile,"%d",&gpiovalue);
 
 Close (fdfile);
 
 if(ret!=1)
   gpiovalue=-1;
 
 
#endif
 return gpiovalue;
}
/*****************************************************************************************
*******************************FINE GPIO *************************************************
******************************************************************************************
*/

/* inizialize()
 * void
 *
 */
 
eth_nodo *inizializeETH(void)
{
		   /* create a new instance */

eth_nodo *gw=NULL;	   
		   
  gw =(eth_nodo *)calloc(1,sizeof(eth_nodo));
  
	 memset((gw->sendBuffer),0,sizeof(gw->sendBuffer));
	  memset((gw->receiveBuffer),0,sizeof(gw->receiveBuffer));
	  memset((gw->nome),0,sizeof(gw->nome));
		
		memset(&(gw->servaddr),0,sizeof(gw->servaddr));
		memset(&(gw->cliaddr),0,sizeof(gw->cliaddr));
		memset(&(gw->broadcast),0,sizeof(gw->broadcast));
		memset(&(gw->ifr),0,sizeof(gw->ifr));
	
		gw->sockfd=0;
		gw->stato=0;
		gw->stato_dhcp=0;
		gw->modo=0;
		gw->subnet=0;
		gw->ticks=0;
		gw->timer=0;
		memset(&(gw->clilen),0,sizeof(socklen_t));
		memset(&(gw->broadlen),0,sizeof(socklen_t));


	
/************INIZIALIZZAZIONE*DO/DI**con*GPIO******************************/	
	initGPIO(GPIO133,GPIOIN);
  initGPIO(GPIO134,GPIOIN);
  initGPIO(GPIO135,GPIOOUT);
  initGPIO(GPIO136,GPIOOUT);
  command_backupmode(NORMALMODE,GPIO135);
  command_backupmode(NORMALMODE,GPIO136);
/**************************************************************************/  
	  
	  return gw;
}

/* check_and_set_host()
 * ret
 * setta: subnet,modo,nome
 */
 
 int check_and_set_host(eth_nodo *gw,unsigned char ncarro,char modo)
 {
 	int ret=-1;
char subnetcarro[3];

	if((gw->stato==0) && (gw->stato_dhcp==0))
	{	
		memset((subnetcarro),0,sizeof(subnetcarro));
		
		gw->subnet=(int)ncarro; // setto la subnet
		gw->modo=modo;   // setto se sono UP o DOWN
		
		
   sprintf(subnetcarro,"%d",gw->subnet); /* setto il nome GATEWAY-NUMCARO*/
   memcpy(gw->nome,"GATEWAY-000",11);
   memcpy(gw->nome+8,&(gw->modo),1);  
	 memcpy((gw->nome)+8+3-strlen(subnetcarro),subnetcarro,strlen(subnetcarro));
	
	//  esempio GATEWAY-D09  :  Gateway della linea di Down del carro n°9
	
	ret=0;
	
  }
  
  
  
  
	return ret;
 	
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

/* dhcp_manage()
 * ret: 
 * comando =1 accendi
 * comando =0 spegni
 */
int dhcp_manage(eth_nodo *gw,int comando)
{
	int ret=-1;
	
	/* abilita il  dhcp */
	if(comando==START && gw->stato_dhcp==0)
		{
			
			system("/etc/init.d/dhcp-server start"); 
			gw->stato_dhcp=1;
			ret=0;
		}
		
		/* abilita il  dhcp anche se è gia acceso */
	if(comando==START && gw->stato_dhcp==1)
		{
			ret=0;
		}
		
		/* disabilita il  dhcp */
	if(comando==STOP && gw->stato_dhcp==1)
		{
			
			system("/etc/init.d/dhcp-server stop"); 
			gw->stato_dhcp=0;
				ret=0;
		}	
	
	/* disabilita il  dhcp anche se è già spento*/
	if(comando==0 && gw->stato_dhcp==0)
		{
			
				ret=0;
			
		}	
		
		/* abilita il  dhcp anche se è già acceso*/
	if(comando==RESTART && gw->stato_dhcp==1)
		{
			gw->stato_dhcp=1;
			system("/etc/init.d/dhcp-server restart"); 
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
	     char *p;
	 int on=1;
	 struct sockaddr_in mem;
	 char IP_GATE[14];
	 char IP_BROADCAST[14];
	 char subnet[3];


	/*crea indirizzo ip GATEWAY e BROADCAT*/
		  memset(&(mem),0,sizeof(mem));
		  memset((IP_GATE),0,sizeof(IP_GATE));
      memset((IP_BROADCAST),0,sizeof(IP_BROADCAST));
       memset(subnet,0,sizeof(subnet));
       
	    memcpy(IP_GATE,"192.168.",8);
	    sprintf(subnet,"%d",gw->subnet);
	    memcpy(IP_GATE+8,subnet,strlen(subnet));
	  	
	  	memcpy(IP_BROADCAST,IP_GATE,strlen(IP_GATE));

	  if((memcmp(&(gw->modo),"D",1) == 0))	
	  	 memcpy(IP_GATE+8+strlen(subnet),".1",2);  
	 
	  if((memcmp(&(gw->modo),"U",1) == 0))	 
			memcpy(IP_GATE+8+strlen(subnet),".2",2);  
 	    
 	    memcpy(IP_BROADCAST+8+strlen(subnet),".255",4);  


	 err_msg ("Subnet:%d", gw->subnet);
	 err_msg ("IP_GATE:%s",IP_GATE);
	 err_msg ("IP_BROADCAST:%s",IP_BROADCAST);
	
	/* create socket */
  gw->sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
	
	/* set socket to sending  BROADCAST  MODE*/
	 setsockopt(gw->sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
	 
	 
   strncpy((gw->ifr).ifr_name, IFNAME, IFNAMSIZ);
   /* indirizzo di ascolto    */
	  (gw->servaddr).sin_family = AF_INET;
	  (gw->servaddr).sin_port = htons(GW_PORT);
	  
	  
	  /* indirizzo statico interfaccia eth */      
       mem=(gw->servaddr);
      (gw->servaddr).sin_addr.s_addr=htonl(INADDR_ANY);
       inet_aton(IP_GATE,&mem.sin_addr);
	  
	
	 // (gw->servaddr).sin_addr.s_addr = inet_addr(HOST);
	 
	  /* indirizzo di destinazione broadcast    */    
     (gw->broadcast).sin_family=AF_INET;
     (gw->broadcast).sin_port=htons(GW_PORT);  //porta di lettura dei client differenziata 5001
     inet_aton(IP_BROADCAST,&(gw->broadcast).sin_addr);
	 
	
	  p = (char *)&mem;
	  memcpy( (((char *)&(gw->ifr) + ifreq_offsetof(ifr_addr) )),p, sizeof(SA));
	  ioctl(gw->sockfd,SIOCSIFADDR,&(gw->ifr));
	  ioctl(gw->sockfd, SIOCGIFFLAGS, &(gw->ifr));
	  (gw->ifr).ifr_flags |= IFF_UP | IFF_RUNNING;
	  // ifr.ifr_flags &= ~selector;  // unset something
	  ioctl(gw->sockfd, SIOCSIFFLAGS, &(gw->ifr));
	  set_no_blocking(gw->sockfd);
      
    Bind (gw->sockfd, (SA*) &(gw->servaddr), sizeof(gw->servaddr));
    
	//		sethostname(gw->nome,10);
			
	//		gethostname(gw->nome,10);
	
	
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
int serverUDP(eth_nodo *gw,int DimWrite)
{
	int ret=-1;
	int dimread=0;
	char tempBuffer[100];
	
	
	memset(tempBuffer,0,sizeof(tempBuffer));
	
	if(gw->stato==0)
			init_serverUDP(gw);
	
	if(gw->stato==1)
		{
			
			#ifdef ENABLEDHCP
					if(dhcp_manage(gw,START))
					   err_sys ("(%s) error - dhcp_manage() failed", prog);
			#endif		   
					gw->clilen = sizeof(gw->cliaddr);
					
					memset((gw->receiveBuffer),0,sizeof(gw->receiveBuffer));
					
					dimread = Recvfrom(gw->sockfd,tempBuffer, DIMPACK, 0, (SA*) &(gw->cliaddr), &(gw->clilen));
				//	info_msg ("tempBuffer:%s", tempBuffer);
					if(dimread==DIMPACK)
						{
							// si devono fare ulteriori controlli nel pck per determinare se deve andare
							// verso la loco o verso la coda (check subnet destinazione e sorgente)
							memcpy(gw->receiveBuffer,tempBuffer,dimread);
							// mantengo il vecchio pak sull'input buffering
							ret=1;
						}
						else
							{
								ret=0;
							}
							
					gw->broadlen = sizeof(gw->broadcast);

					
					if(DimWrite)
						{
						 Sendto (gw->sockfd, gw->sendBuffer, DimWrite, 0, (SA*) &(gw->broadcast),gw->broadlen);
	//				  		 info_msg ("Inviato a client%s %u",inet_ntoa((gw->broadcast).sin_addr), ntohs((gw->broadcast).sin_port));
   
					   }
					
			    memset((gw->sendBuffer),0,sizeof(gw->sendBuffer));
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
		int ret=-1;
		static unsigned char cont=8;

	static unsigned char contami=0;
	 
	 memset((gate->receiveBuffer),0,sizeof(gate->receiveBuffer));
	contami++;
	
	if(contami%5){
		cont++;
		sprintf(gate->receiveBuffer,"%d",cont);
		
		memcpy(gate->receiveBuffer+strlen(gate->receiveBuffer),"--",3-strlen(gate->receiveBuffer));
		memcpy(gate->receiveBuffer+3,"-",1);
		memcpy(gate->receiveBuffer+3+1,gate->nome,11);
		memcpy(gate->receiveBuffer+3+1+11,"-SONO-UN-PACCHETTO-ETHERNETSONO-UN-PACCHETTO--SE-FUNGO\n",55);
	//	55+ 11+3+1+1= 70memset(&(gate->receiveBuffer),0,4);
 }
		return ret;
}


/*
 * Check_BACKUPMODE()
 *elimina i dati
 *
*/
int Check_BACKUPMODE(eth_nodo *gw,char *NI ,char *buffETHTX)
{
	int ret=-1;
	
	#ifdef ENABLEGPIO
	int isBackup_Slave=0,isBackup_Master=0;
	int IsSlave=0,IsMaster=0,isDown=0,isUP=0;
		int IsBackup_MySlave=0,IsBackup_MyMaster=0;
	int isIPGateway=0;
	char sep[]="."; 
  char *ultimocampo=0;
  
  IsBackup_MySlave=readGPIO(GPIO135); /*SLAVE my GW bitinfo backup*/
  IsBackup_MyMaster=readGPIO(GPIO136);/*MASTER my GW bitinfo in backup*/
	
	isBackup_Slave=readGPIO(GPIO134); /*SLAVE dell'altro GW bitinfo backup*/
  isBackup_Master=readGPIO(GPIO133);/*MASTER dell'altro GW bitinfo in backup*/
  
   
IsMaster = (memcmp(NI,"MASTE-",6)==0);
 IsSlave = (memcmp(NI,"SLAVE-",6)==0);
  
  
  isDown = (memcmp(&gw->modo,"D",1)==0);
  isUP = (memcmp(&gw->modo,"U",1)==0);

/* prelevo l'ultimo campo dell'indirizzo IP sorgente del pacchetto ETH appena ricevuto*/	
	  ultimocampo=strtok(inet_ntoa((gw->cliaddr).sin_addr),sep);
    ultimocampo=strtok(NULL,sep);
    ultimocampo=strtok(NULL,sep);
    ultimocampo=strtok(NULL,sep);
 
 isIPGateway=((memcmp(ultimocampo,"1",1)==0)|| (memcmp(ultimocampo,"2",1)==0));

//MODO NORMALE
	if((IsMaster) && (isDown) && (!isBackup_Master)) 
		{
			 memset(buffETHTX,0,70);
		}
	
		if((IsSlave) && (isUP) && (!isBackup_Slave)) 
		{
			 memset(buffETHTX,0,70);
		}	
	
	//i trasmettitori in normal mode non trasmettono pck arrivati dai gateway
	//tranne se il loro ricevitore non è in stato di failure
	
	if((IsSlave) && (isDown)&& (!IsBackup_MyMaster)  && (isIPGateway)) 
		{
			 memset(buffETHTX,0,70);
			 /* per questo ciclo lo slave non trasmette niente */
		}
		
	if((IsMaster) && (isUP) && (!IsBackup_MySlave)  && (isIPGateway)) 
		{
			 memset(buffETHTX,0,70);
			 /* per questo ciclo il master non trasmette niente */
		}	
	
	#endif
	return ret;
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
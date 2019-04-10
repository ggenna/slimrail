#ifndef GESTIONETH_H
#define GESTIONETH_H


#include <stdarg.h>
#include <unistd.h>
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
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>




/********************ABILITAZIONE*******************************/

//#define ENABLEGPIO 1
#define ENABLEETH 1
//#define ENABLEDHCP 1
//modo trasferimento
//D DOWN x.x.x.1
//U UP   x.x.x.2
/*******************INTERFACCIA FISICA*******************************/
#define IFNAME "usb0"


#define BACKUPMODE (1)
#define NORMALMODE (0)
#define GPIOOUT (1)
#define GPIOIN (0)

/*gpio-pin: 133-15,134-13,135-11,136-9*/
/*   collegamento fisico BBxM1_GPIO133 <--- BBxM2_GPIO136    */
/*   collegamento fisico BBxM1_GPIO136 ---> BBxM2_GPIO133    */
/*riferim: input  xbee1 */
#define GPIO133    (133)
/*riferim: output  xbee1 */
#define GPIO136    (136)

/*   collegamento fisico BBxM1_GPIO134 <--- BBxM2_GPIO135    */
/*   collegamento fisico BBxM1_GPIO135 ---> BBxM2_GPIO134    */
/*riferim: input  xbee2 */
#define GPIO134    (134)
/*riferim: output  xbee2 */
#define GPIO135    (135)

typedef struct {
  
  int sockfd;
  int stato; // socket  aperto=1 o chiuso=0
  int stato_dhcp; // dhcp  abilitato=1 o non abilitato=0
  char modo; // 0=defaul UP=U o Down=D
  int subnet;// 0=X default: numerazione della subnet 192.168.X.1, indica il numero del carro
  struct sockaddr_in servaddr, cliaddr,broadcast;
  socklen_t clilen,broadlen;
  time_t ticks;
  char nome[11];
char sendBuffer[70];
char receiveBuffer[70];
char *punt; // da eliminare
  struct ifreq ifr;
  unsigned int timer;  
  
  
}eth_nodo;

/* ordine di chiamata delle funzine che gestiscono la porta ethernet*/
eth_nodo *inizializeETH(void); // una sola volta
int check_and_set_host(eth_nodo *gw,unsigned char ncarro,char modo);// una sola volta
int serverUDP(eth_nodo *gw,int DimWrite);// nel loop
int command_backupmode(int mode,int gpio);
int readGPIO(int gpio);
int Check_BACKUPMODE(eth_nodo *gw,char *NI,char *buffETHTX);

 int provalo(eth_nodo *gate);


#endif
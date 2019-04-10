#ifndef INTERFACCIAETH_H
#define INTERFACCIAETH_H


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

#define BACKUPMODE (1)
#define NORMALMODE (0)
#define GPIOOUT (1)
#define GPIOIN (0)

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
  
  int sockfdtreno,sockfdterra;
  int stato; // socket  aperto=1 o chiuso=0
  struct sockaddr_in servtreno,servterra, broadcast,cliaddr;
  socklen_t clilen,broadlen;
  time_t ticks;
  char nome[11];
char sendBufferTreno[70];
char receiveBufferTreno[70];
char sendBufferTerra[70];
char receiveBufferTerra[70];
char *punt; // da eliminare
  struct ifreq ifrtreno,ifrterra;
  unsigned int timer;  
  
  
}eth_nodo;

/* ordine di chiamata delle funzine che gestiscono la porta ethernet*/
eth_nodo *inizializeETH(void); // una sola volta
int serverUDP(eth_nodo *gw,int DimWriteTreno,int DimWriteTerra);// nel loop


 int provalo(eth_nodo *gate);
unsigned int setTimer(unsigned int tempo);
void stampatimestamp(time_t timestamp,suseconds_t time);

#endif
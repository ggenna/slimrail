
#include <stdio.h>
#include <stdlib.h>

#include <stdarg.h>

#include <string.h>
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

#define MAXNODIDISCOVERY 4




typedef struct{
  char my[5];
  char addrSourceSH[9];
   char addrSourceSL[9];
  char NI[8]; //SLAVE-x o MASTE-x
 char parentN[5];
  char devtype[3]; 
 char status[3];
 char profile[5];
 char manufac[5];


}xbee_nodo;

typedef struct {
  
  int ttyfd;
  struct termios  status;
 char version[2];
 char NI[8];
 int type; // 1 master 2 slave
 int link;//1=attivo 2= non attivo
  char *path; /* serial port path */
 unsigned char carro;
 char modotrasferimento; // UP - DOWN - FULLDUPLEX
 char modoAT; // 1 si, 0 no
int fase;
enum modo{ CONNESSIONE, ABILITAZIONE,ATTESA,REGIME } mode;
union {
xbee_nodo nodojoin;
char nodediscovery[sizeof(xbee_nodo)+1];


};

char NIxbeeCarro[8];  //NI del xbee sullo stesso carro, se siamo sul locomotore o sul carro di coda è nullo
xbee_nodo nodi[4];   // memorizzo i 4 nodi piu vicini
xbee_nodo connesso;  //nodo con cui è connesso
int nr_nodiscoperti; // numero nodi scoper MAX=4

unsigned int timer;
unsigned int contatore;
long timeout;
int lock;
unsigned int pckpersi;
char bufferRX[70];
char bufferTX[70];
unsigned char lockwrite;
 //  struct xbee_hd next;
}xbee_hnd;





int init_serial(xbee_hnd *xbee, int baudrate);







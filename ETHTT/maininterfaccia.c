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
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/select.h>

//#include "sockwrap.h"

#include "errlib.h"
#include "miowrap.h"
#include "interfacciaeth.h"

char *prog;


/*
 *
 * MAIN
 *
 */
int main(int argc, char *argv[]) {
	

//ETHERNET
eth_nodo *gateway=NULL;
char bufferethernetTX[70];
gateway=inizializeETH();
 memset(bufferethernetTX,0,sizeof(bufferethernetTX));
 int diminvio=0;

/********************************************************************/


		
		info_msg ("GESTIONE ETHERNET");	
		
		//gateway->timer=setTimer(120);	

		// copio il buffer di ricezine di xbeecurr  nel buffer TX ethernet
 //   memcpy(gateway->sendBuffer,xbeecurr->bufferRX,strlen(xbeecurr->bufferRX));
		
				
while(1)
{
   
    //TEMPORANEO PER TEST 
    
    //copio il buffer RX ethernet in buffer di trasmissione xbee1
    // eseguo la write sul sock xbee1
  
     memset(bufferethernetTX,0,sizeof(bufferethernetTX));
     
      info_msg ("diminvio:%d",diminvio);

     if((serverUDP(gateway,0,diminvio))>0)
     	{
			
        diminvio=provalo(gateway);
        
      }
//       info_msg ("NIxbeeCarro:%s\nNI%s\nsemicarro:%d",xbeecurr->NIxbeeCarro,xbeecurr->NI,temp_semicarro);

//     memcpy(bufferethernetTX,gateway->receiveBuffer,sizeof(bufferethernetTX));
 //    info_msg ("TXfromETH:%s len:%d",bufferethernetTX,strlen(gateway->receiveBuffer));




}


return (0);
}








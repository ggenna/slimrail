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
/*
inseriti in piu da giovanni

*/
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/select.h>

//#include "sockwrap.h"

#include "errlib.h"
#include "apiMio.h"
#include "miowrap.h"
#include "slimportXbee.h"
#include "GestioneETH.h"

char *prog;
//long TIMEOUT;

/*
devo controllare fd vecchio stato porta

tcgetattr(fd,&oldtio); // save current port settings 

ce alla chiusura

// restore old port settings

	tcsetattr(fd,TCSANOW,&oldtio);


*/


/*
 *
 * MAIN
 *
 */
int main(int argc, char *argv[]) {
	
//	 char risposta[9];

unsigned int time;
  xbee_hnd *xbee1;
  xbee_hnd *xbee2;
  xbee_hnd *xbeecurr, *xbeetemp;
int ret=0,ris=0;
int conf=6;


//char Inseriscipath[12];
//char buffer[70];
//int nbytes=0;

int dimensione=0;

//*******************************************************************************************************

int temp_semicarro=0;  // variabile da sostituire con una lettura hardware dinamica

//semicarro =1 sono locomotore
//semicarro =2 sono carro coda
//semicarro =0 non sono semicarro ma un carro con master e slave

//*******************************************************************************************************

//ETHERNET
eth_nodo *gateway=NULL;
char bufferethernetTX[70];
gateway=inizializeETH();
 memset(bufferethernetTX,0,sizeof(bufferethernetTX));

sleep(7);
/********************************************************************/


sscanf(argv[1],"%d",&temp_semicarro);


  if ((xbee1=xbee_setup("/dev/ttyUSB0",115200)) ==NULL) {
    return 1; //ret=1;

  }
  memcpy(xbee1->NI,"DEFAULT",7);
  memcpy(xbee1->NIxbeeCarro,"DEFAULT",7);
  xbee1->modoAT=0;
  xbee1->timer=0;
  xbee1->timeout=5;
  xbee1->lock=0;
  xbee1->lockwrite=0;
  xbee1->contatore=0;
  xbee1->pckpersi=0;
  xbee1->modotrasferimento=0;
      
  memset(xbee1->bufferRX,0,sizeof(xbee1->bufferRX));
  memset(xbee1->bufferTX,0,sizeof(xbee1->bufferTX));
  //xbee1->bufferTX=malloc(70*sizeof(char));
  

 // (xbee1->bufferTX)=(xbee1->bufferRX); // DA ELIMINARE SOLO PER TEST

if(temp_semicarro==0)
{
	
	  if ((xbee2=xbee_setup("/dev/ttyUSB1",115200)) ==NULL) {
	    return 1; //ret=1;

	  }
	  memcpy(xbee2->NI,"DEFAULT",7);
	  memcpy(xbee2->NIxbeeCarro,"DEFAULT",7);
    xbee2->modoAT=0;
    xbee2->timer=0;
    xbee2->timeout=5;
    xbee2->lock=0;
    xbee2->contatore=0;
    xbee2->pckpersi=0;
    xbee2->lockwrite=0;
    xbee2->modotrasferimento=0;
    memset(xbee2->bufferRX,0,sizeof(xbee2->bufferRX));
     memset(xbee2->bufferTX,0,sizeof(xbee2->bufferTX));
 //   xbee2->bufferTX=malloc(70*sizeof(char));
 //  (xbee2->bufferTX)=(xbee1->bufferRX);
 //  (xbee1->bufferTX)=(xbee2->bufferRX);
}
else // se sono in coda o in testa
  {
  xbee2=(xbee_hnd *)calloc(1,sizeof(xbee_hnd));	
  memset(xbee2->bufferRX,0,sizeof(xbee2->bufferRX));
  memset(xbee2->bufferTX,0,sizeof(xbee2->bufferTX));
 //  xbee2->bufferTX=malloc(70*sizeof(char));
  

  }


/*
 if((ris=init_modem(xbee1))==0) 
{
 err_msg ("Modem inizializzato con descrittore:%d", xbee1->ttyfd);
 xbee1->modoAT=1;
}
else
{
  err_msg ("ERRORE Modem non inizializzato\n");
}
*/


 if(temp_semicarro==1) // se sono sul locomotore
  {

	/*  if((commandAT(xbee1,"NISLAVE-0",NULL))==0)  // mi assegno il nome
	   {
		err_msg ("comando NISLAVE-0  OK");
		*/

	memcpy(xbee1->NI,"SLAVE-0",7);
	memcpy(xbee1->NIxbeeCarro,"DEFAULT",7);
	// inizializzo modotrasferimento
	memcpy(&(xbee1->modotrasferimento),"D",1); 
		
	/*   }
	   else
	   {
		 err_msg ("comando NISLAVE-0 fallito ");
	   }
	  */ 
  
  }
  

/*
 if(xbee1->modoAT==1)
 {
	  
	  if((ris=release_modem(xbee1))==0) 
		{
		 err_msg ("Modem rilasciato, modalita' trasparente abilitata:%d", xbee1->ttyfd);
		  xbee1->modoAT=0;
		}
		else
		{
		  err_msg ("ERRORE Modem non Rilasciato\n");
		}

}
*/

/*
if(temp_semicarro==0)
{

	 if((ris=init_modem(xbee2))==0) 
	{
	 err_msg ("Modem inizializzato con descrittore:%d", xbee2->ttyfd);
	  xbee2->modoAT=1;
	}
	else
	{
	  err_msg ("ERRORE Modem non inizializzato\n");
	}
	
}

*/

/*
 * invio comando AT con risposta ok
 *
 * /

 if((ris=commandAT(xbee1,""))==0) 
{
 err_msg ("comando eseguito");
}
else
{
  err_msg ("comando fallito\n");
}
*/

/**
 * esce dalla modalità comando
 * 
 * /
 if((ris=release_modem(xbee1))==0) 
{
 err_msg ("Modem rilasciato, modalita' trasparente abilitata:%d", xbee1->ttyfd);
}
else
{
  err_msg ("ERRORE Modem non Rilasciato\n");
}
*/


xbeecurr=xbee1;

xbee1->mode=CONNESSIONE;

 if(temp_semicarro==0)
    xbee2->mode=CONNESSIONE;

sleep(5);
while(1)
{



 time=setTimer(0);

 
 


 
 	 if((temp_semicarro==1) ||(temp_semicarro==2))
 	 	{
 	 	    xbeecurr=xbee2;
	    
 	 	  }
 	 	 
 	 	 
	if((xbeecurr==xbee1))
	{
		 memcpy(xbee1->bufferTX,xbee2->bufferRX,sizeof(xbee1->bufferTX));
	  
	 if(xbeecurr->lock==0)
     xbeecurr=xbee2;
      
      // questo e' necessario per eliminare un modulo che non riesce a connettersi con nessuno
      
	
	  if((xbeecurr->timer <=time)&&((xbeecurr->contatore)<5))
	   { 
	     xbeecurr->timer=setTimer(100);	
	     if(xbeecurr->lock==0)
	      info_msg ("****************** DIVENTO XBEE_2 *******************************");   
	      else
	      	info_msg ("****************** LOCK XBEE_1 *******************************");   
	      	
	   
	   }
	   else
	   {
	     //  info_msg ("**************** CICLO IDLE XBEE_2 *****************************");
	       continue;
	   }
	  
	}
	else if((xbeecurr==xbee2))
	{  
	
	if(temp_semicarro==0)
 	 	{	
	        memcpy(xbee2->bufferTX,xbee1->bufferRX,sizeof(xbee2->bufferTX)); 
      }
      
      
	if(xbeecurr->lock==0)
	  xbeecurr=xbee1;
	  // questo e' necessario per eliminare un modulo che non riesce a connettersi con nessuno

     	  
	  if((xbeecurr->timer <=time)&&((xbeecurr->contatore)<5))
	   { 
	     xbeecurr->timer=setTimer(100);	
	     	if(xbeecurr->lock==0)
	     info_msg ("****************** DIVENTO XBEE_1 *******************************");   
	    else
	    	info_msg ("****************** Lock XBEE_2 *******************************");   
	   
	   }
	   else
	   {
	    //  info_msg ("**************** CICLO IDLE XBEE_1 *****************************");
	       continue;
	   }
	}	
	else
	{	
	 info_msg ("****************** NON SONO NIENTE *******************************");
	 exit(1);
	}



switch(xbeecurr->mode)
{
	case CONNESSIONE:info_msg ("CONNESSIONE\n");
			dimensione=19; // 18 byte invio e ricevo
	break;
	case REGIME:info_msg ("REGIME\n");
		    dimensione=71; // 70 byte invio e ricevo

		   // exit(0);
	break;
	
	default: dimensione=19;
		break;

}


switch((conf=Config_MasterSlave(xbeecurr,temp_semicarro)))
{
	case 0:info_msg ("Io sono SLAVE-x");
	        	
	          	
	        	
	      if(xbeecurr->mode!=REGIME)
	       {
	       	if((gestione_Connessione_Slave(xbeecurr,dimensione))==0)
	       			info_msg ("STO ANDANDO A REGIME");
	       	else
	       		{
	       			//xbeecurr->timer=setTimer(500);
						  // xbeecurr->contatore=(xbeecurr->contatore) +1;
						  info_msg ("NON SONO A REGIME");
	       			
	       		}
	       	}
	       	
	       	
	        if(xbeecurr->mode==REGIME)
	       	{
	       		  /* GPIO cancella quando necessario il bufferethernetTX */
	       
	       if((*bufferethernetTX)!=0)		  
	       	  Check_BACKUPMODE(gateway,xbeecurr->NI,bufferethernetTX);     
	       	  
		       			info_msg ("SONO IN REGIME");
	             	if(gestioneSlave(xbeecurr,dimensione,bufferethernetTX)==0)
	             		{
	             			
						     		    xbeecurr->pckpersi=0;
						     		      info_msg ("PCK OK");
						     		    command_backupmode(NORMALMODE,GPIO135);
	             		}
	             		else
	             			{
	             				  info_msg ("PCK PERSI");
	             				xbeecurr->pckpersi=xbeecurr->pckpersi +1;
						     		if(xbeecurr->pckpersi>500)
						     			{
						     				command_backupmode(BACKUPMODE,GPIO135);
						     		//		xbeecurr->mode=ATTESA;
						     		//		xbeecurr->timeout=4;
						     		//		xbeecurr->pckpersi=0;
						     			}
	             			}
	       	
	        }
	      /*
	       	memset(&buffer,0,sizeof(buffer));
	       	while ((nbytes = read(xbeecurr->ttyfd, buffer, 1)) > 0)
	       	   err_msg ("buffer  :%s %d",buffer,nbytes);
	*/
	break;
	

	case 2:info_msg ("Io sono MASTE-x");
		
		
				 if(xbeecurr->mode==REGIME)
	       {
	       	
	       	

	       	
	       	       info_msg ("SONO A REGIME");
	       	  
	       	  /* GPIO cancella quando necessario il bufferethernetTX */
	       	if((*bufferethernetTX)!=0) 
	       	  Check_BACKUPMODE(gateway,xbeecurr->NI,bufferethernetTX);     
	       	       
						     if(gestioneMaster(xbeecurr,dimensione,bufferethernetTX)==0)
						     	{
						     		    xbeecurr->pckpersi=0;
						     //		    xbeecurr->lock=0;
						     		     info_msg ("PCK OK");
						     		   command_backupmode(NORMALMODE,GPIO136);
						       
						     	}
						     else
						     	{
						     		   info_msg ("PCK PERSI");
						     		xbeecurr->pckpersi=xbeecurr->pckpersi +1;
						     		if(xbeecurr->pckpersi>500)
						     			{
						     	command_backupmode(BACKUPMODE,GPIO136);
						    
						     	//			xbeecurr->mode=ATTESA;
						     	//			xbeecurr->timeout=4;
						     	//			xbeecurr->pckpersi=0;
						     			}
						     		
						     	}
						     	
		          	
		     }
		
		 if(xbeecurr->mode!=REGIME)
	       {
	       	 info_msg ("NON SONO A REGIME");
		     	gestione_Connessione_Master(xbeecurr,dimensione);
		     	
	      }
		
		     
	      	    
	break;
	
	case 3:info_msg ("Il mio compagno di carro deve essere settato come SLAVE");
	       // 	In questa sezione sono sempre in xbee->mode=ATTESA;
	       
					info_msg ("Assegno il nominativo al carro");
				//	sleep(5);
					
					if(xbeecurr==xbee1)
						xbeetemp=xbee2;
					else
						xbeetemp=xbee1;
					
				
        	
					if(assegna_slave_carro(xbeetemp,xbeecurr)==0)
						{
							info_msg ("TUTTO OK ");
							xbeecurr->lock=0;
						}
						
				 //   sleep(15);
	 	
	  	
	break;
	
	case 4:info_msg ("Carro non inizializzato");
			
		
			
	 if(xbeecurr->mode==CONNESSIONE)
	   {			
						if((ris=scanning(xbeecurr,"ND"))==1) 
						{
							/*
						 err_msg ("XBBE con ID=234 con descrittore:%d", xbeecurr->ttyfd);
			
						err_msg ("\n  MY:%s", (xbeecurr->nodojoin).my);
			
						err_msg ("ADDR:%s%s", (xbeecurr->nodojoin).addrSourceSH,(xbeecurr->nodojoin).addrSourceSL);
						err_msg ("  NI:%s", (xbeecurr->nodojoin).NI);
						err_msg ("PNET:%s", (xbeecurr->nodojoin).parentN);
						err_msg ("DEVT:%s", (xbeecurr->nodojoin).devtype);
						err_msg ("STAT:%s", (xbeecurr->nodojoin).status);
						err_msg ("P_ID:%s", (xbeecurr->nodojoin).profile);
						err_msg ("M_ID:%s", (xbeecurr->nodojoin).manufac);
			
			*/
						}
						else if(ris>1)
						{
						  err_msg ("ERROR scanning ");
			
						}
						else
						{
						   err_msg ("ERRORE Modem non connesso");
						}
																	
						if((ris=check_and_select(xbeecurr))==0)
						{
						    info_msg ("comunicazione avviata");
						    xbeecurr->lock=1;
			
						}
						else
						{
						   xbeecurr->timer=setTimer(10000);
						   xbeecurr->contatore=(xbeecurr->contatore) +1;
						    info_msg ("selezione non avvenuta, attento 5 sec");
			
						}
	
	
	
	    sleep(5);
		}
	
	
	break;
	
	default: info_msg ("Errore!!!!!! :%d",conf);
		exit(1);
	break;

}




// GESTIONE ETHERNET

// inserire un time per lo scheduling
// controllo se sono  REGIME e se sono sul carro intermedio o di testa
if((((xbee1->mode==REGIME)&&(temp_semicarro!=0))||((xbee1->mode==REGIME)&&(temp_semicarro==0)&&(xbee2->mode==REGIME)))&&(xbeecurr->modotrasferimento!=0))
	{
		
	
	//	if(gateway->timer <= time)
	//	{
		
		info_msg ("GESTIONE ETHERNET");	
		
		//gateway->timer=setTimer(120);	
		info_msg ("xbeecurr->carro:%d xbeecurr->modotrasferimento:%c temp_semicarro:%d",xbeecurr->carro,xbeecurr->modotrasferimento,temp_semicarro);


		check_and_set_host(gateway,(unsigned char)(xbeecurr->carro),(char)(xbeecurr->modotrasferimento));

		// copio il buffer di ricezine di xbeecurr  nel buffer TX ethernet
	if(strlen(xbeecurr->bufferRX)>=1)	
    memcpy(gateway->sendBuffer,xbeecurr->bufferRX,sizeof(xbeecurr->bufferRX));
		// info_msg ("gateway->sendBuffer:%s DIM:%d",gateway->sendBuffer,strlen(gateway->sendBuffer));

		
		
		
			#ifdef ENABLEETH	
			/*SlimRail se il carro trasmette in eth*/
			if(temp_semicarro==1)	
	    	serverUDP(gateway,sizeof(gateway->sendBuffer));
	    else
	    	serverUDP(gateway,0);
	    	
      #else 
      provalo(gateway); //TEMPORANEO PER TEST 
      #endif
    //copio il buffer RX ethernet in buffer di trasmissione xbee1
    // eseguo la write sul sock xbee1
  
     memset(bufferethernetTX,0,sizeof(bufferethernetTX));
//       info_msg ("NIxbeeCarro:%s\nNI%s\nsemicarro:%d",xbeecurr->NIxbeeCarro,xbeecurr->NI,temp_semicarro);


/* Per SLimRail se sono il loomotore non trasmetto niente su wireless che proviene da ETH*/
    if(temp_semicarro!=1)
       memcpy(bufferethernetTX,gateway->receiveBuffer,sizeof(bufferethernetTX));
     
 //    memset(gateway->receiveBuffer,'\0',strlen(gateway->receiveBuffer));
 //    info_msg ("TXfromETH:%s len:%d",bufferethernetTX,strlen(gateway->receiveBuffer));

//    info_msg ("NIxbeeCarro:%s\nNI%s\nsemicarro:%d",xbeecurr->NIxbeeCarro,xbeecurr->NI,temp_semicarro);

//}


		
		
	}





}





/**
* eseguo lo scanning
*/


/*
 if((ris=scanning(xbee1,"ND"))==0) 
{
 err_msg ("XBBE con ID=234 con descrittore:%d", xbee1->ttyfd);

err_msg ("\n  MY:%s", (xbee1->nodojoin).my);

err_msg ("ADDR:%s%s", (xbee1->nodojoin).addrSourceSH,(xbee1->nodojoin).addrSourceSL);
err_msg ("  NI:%s", (xbee1->nodojoin).NI);
err_msg ("PNET:%s", (xbee1->nodojoin).parentN);
err_msg ("DEVT:%s", (xbee1->nodojoin).devtype);
err_msg ("STAT:%s", (xbee1->nodojoin).status);
err_msg ("P_ID:%s", (xbee1->nodojoin).profile);
err_msg ("M_ID:%s", (xbee1->nodojoin).manufac);


}
else if(ris>0)
{
  err_msg ("cambio CS ");

}
else
{
   err_msg ("ERRORE Modem non connesso");
}

if((ris=check_and_select(xbee1))==0)
{
    err_msg ("comunicazione avviata");

}
else
{
    err_msg ("selezione non avvenuta");

}
return 0;


*/



/* leggo sul primo parametro e scrivo sul secondo parametro che passo 80 byte */

/*

	
   err_msg ("scrivo 80\n"); 
    write(xbee2->ttyfd, "SONO-SINCRONIZZATO  SONO-SINCRONIZZATO  SONO-SINCRONIZZATO  SONO-SINCRONIZZATO      \r", 85);

ret=0;
 memset(&buffer,0,sizeof(buffer));	
while(ret==0){

	if((nbytes = Readlinetime(xbee1->ttyfd, buffer, 90)) > 0)
	{
	prova++;
            err_msg ("%d %d Buffer:%s",prova,nbytes,buffer);
             
                    write(xbee2->ttyfd, "SONO-SINCRONIZZATO  SONO-SINCRONIZZATO  SONO-SINCRONIZZATO  SONO-SINCRONIZZATO      \r", 85);


	}
	else if(nbytes==-3)
	{
	     err_msg ("Buffer non sincronizzato");
	    write(xbee2->ttyfd, " NON-SONO-SINCRONIZZATO   NON-SONO-SINCRONIZZATO   NON-SONO-SINCRONIZZATO   NON-SONO-SINCRONIZZATO \r", 85);
	  
	}
	else
	{
	  err_msg ("non definito");
	  ret=2;
	}

}*/














return ret;
}








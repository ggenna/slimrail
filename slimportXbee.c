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
#include "errlib.h"
#include "apiMio.h"


#include <sys/signal.h>
#include <sys/types.h>
#include <sys/select.h>

//#include "sockwrap.h"
#include "miowrap.h"
#include "slimportXbee.h"

char *prog;
//long TIMEOUT;
int DimNodoMio[9]={5,9,9,8,5,3,3,5,5};

  
   /* setup libxbee */
             // char *path, int baudrate, int logfd, char cmdSeq, int cmdTime

xbee_hnd *xbee_setup(char *path, int baudrate)
 {

int ret;
xbee_hnd *xbee=NULL;

   /* create a new instance */
  xbee =(xbee_hnd *)calloc(1,sizeof(xbee_hnd));
  //xbee->next = NULL;

     /* take a copy of the XBee device path */
  if ((xbee->path = (char *)malloc(sizeof(char) * (strlen(path) + 1))) == NULL) {
	//free(xbee->path);      
	//free(xbee);
    return NULL;
  }
  else
  	strcpy(xbee->path,path);

  /* call the relevant init function */
   if ((ret = init_serial(xbee,baudrate)) != 0) {
	//free(xbee->path);
	//free(xbee);
    return NULL;
  }
  
  
    return xbee;
  }


 /* init_modem  O - 0 = MODEM ok, -1 = MODEM bad */
int init_modem(xbee_hnd *xbee)   /* I - Serial port file */
    {
      char buffer[255];  /* Input buffer */

      int  nbytes;       /* Number of bytes read */
      int  tries;        /* Number of tries so far */
      int ret=-1;
      char vmin=0,vtime=0;



vmin=(char)(xbee->status).c_cc[VMIN]; 
vtime=(char)(xbee->status).c_cc[VTIME];



(xbee->status).c_cc[VMIN]  = 2; 
(xbee->status).c_cc[VTIME] = 5; 

	if(tcflush(xbee->ttyfd, TCIOFLUSH)==-1)
	{
		   err_msg ("(%s) tcflush()", prog);
		
	}	
	if(tcsetattr(xbee->ttyfd, TCSANOW, &(xbee->status))==-1) 
	{
		   err_msg ("(%s) tcsetattr()", prog);
		
	}
   
sleep(1);// GT 1000ms
      for (tries = 0; tries < 3; tries ++)
      {

       /* send an AT command followed by a CR */
     if(write(xbee->ttyfd, "+++", 3) < 3)
	continue;
     else
  	{
  	err_msg ("Ho scritto +++ e attendo ...\n");
	break;
	}
      }
sleep(1);// GT 1000ms

       /* read characters into our string buffer until we get a CR or NL */
	//bufptr = buffer;
	//while ((nbytes = read(xbee->ttyfd, bufptr, 3)) > 0)

	if((nbytes = Readlinetime(xbee->ttyfd, buffer, 3,xbee->timeout)) > 0)
	{
     err_msg ("sono arrivati %d dati %s",nbytes,buffer);
	 
	}
	else
	   err_msg ("(%s)Readlinetime()", prog);


       /* nul terminate the string and see if we got an OK response */
	//*bufptr = '\0';

	if (strncmp(buffer, "OK", 2) == 0)
	{
	  ret=0;
	  
         }

	
	(xbee->status).c_cc[VMIN]  = vmin; 
	(xbee->status).c_cc[VTIME] = vtime;

	if(tcflush(xbee->ttyfd, TCIOFLUSH)==-1)
	{
		   err_msg ("(%s) tcflush()", prog);
		ret=-1;
	}	
	if(tcsetattr(xbee->ttyfd, TCSANOW, &(xbee->status))==-1) 
	{
		   err_msg ("(%s) tcsetattr()", prog);
		ret=-1;
	} 






      return ret;
 }


/*release modem*/

 /* init_modem  O - 0 = MODEM ok, -1 = MODEM bad */
int release_modem(xbee_hnd *xbee)   /* I - Serial port file */
    {
      char buffer[255];  /* Input buffer */
int ret=-1;
      int  nbytes;       /* Number of bytes read */
      int  tries;        /* Number of tries so far */
            char vmin=0,vtime=0;



vmin=(char)(xbee->status).c_cc[VMIN]; 
vtime=(char)(xbee->status).c_cc[VTIME];



	if(tcflush(xbee->ttyfd, TCIOFLUSH)==-1)
	{
		   err_sys ("(%s) tcflush()", prog);
		
	}	
	if(tcsetattr(xbee->ttyfd, TCSANOW, &(xbee->status))==-1) 
	{
		   err_sys ("(%s) tcsetattr()", prog);
		
	}


      for (tries = 0; tries < 3; tries ++)
      {

       /* send an AT command followed by a CR */
     if(write(xbee->ttyfd, "ATCN\r", 5) < 5)
	continue;
     else
  	err_msg ("Ho scritto ATCN e attendo ...\n");



	if((nbytes = Readlinetime(xbee->ttyfd, buffer, 3,xbee->timeout)) > 0)
	{
     err_msg ("sono arrivati %d dati %s",nbytes,buffer);
	 
	}
	else
	   err_msg ("(%s)Readlinetime()", prog);


       /* nul terminate the string and see if we got an OK response */
	//*bufptr = '\0';

	if (strncmp(buffer, "OK", 2) == 0)
	  ret=0;
	  
	  break;
      }
      
      
      (xbee->status).c_cc[VMIN]  = vmin; 
	(xbee->status).c_cc[VTIME] = vtime;

	if(tcflush(xbee->ttyfd, TCIOFLUSH)==-1)
	{
		   err_sys ("(%s) tcflush()", prog);
		
	}	
	if(tcsetattr(xbee->ttyfd, TCSANOW, &(xbee->status))==-1) 
	{
		   err_sys ("(%s) tcsetattr()", prog);
		
	} 
      
      

      return ret;
    }


/*
 ret=0 se sono SLAVE 
 ret=1 
 ret=2 se sono Master 
 ret=3 se sono Mastere il mio vicino di carro deve essere settato come Slave
 ret= 4 non inizializzato
 reto=5 non definito (esempio se esiste uno slave e non esiste il master)

semicarro =1 sono locomotore
semicarro =2 sono carro coda
semicarro =0 non sono semicarro ma un carro con master e slave

*/

int Config_MasterSlave(xbee_hnd *xbee,int semicarro)
{

	int ret=5;

  	if((memcmp(xbee->NIxbeeCarro, "DEFAULT", 7) == 0) && (memcmp(xbee->NI, "DEFAULT", 7) == 0))
	{
          
          //err_msg ("Carro non inizializzato");
          ret=4;

	}
	
	/*
	if((memcmp(xbee->NIxbeeCarro, "MASTE-", 6) == 0) && (memcmp(xbee->NI, "DEFAULT", 7) == 0))
	{
          
      //    err_msg ("Io saro' settato come SLAVE-x");
          ret=1;
          
	}*/
	
	if((memcmp(xbee->NIxbeeCarro, "DEFAULT", 7) == 0) && (memcmp(xbee->NI, "MASTE-", 6) == 0)&&(semicarro==0))
	{
         // vale solo per i carri intermedi
         // err_msg ("Il mio vicino di carro deve essere settato come SLAVE");
         // quando instauro il link con conferma nomino anche lo slave che sta sul mio stesso carro
          ret=3;
          
	}
	
	if( ((memcmp(xbee->NIxbeeCarro, "SLAVE-", 6) == 0)|| (semicarro==2)) && (memcmp(xbee->NI, "MASTE-", 6) == 0))
	{
          
        //  err_msg ("Io sono MASTE-x");
          ret=2;
          
	}
	
	if( ((memcmp(xbee->NIxbeeCarro, "MASTE-", 6) == 0)|| (semicarro==1)) && (memcmp(xbee->NI, "SLAVE-", 6) == 0))
	{
          
        //  err_msg ("Io sono SLAVE-x");
          ret=0;
          
	}
	
//	info_msg ("NIxbeeCarro:%s\nNI%s\nsemicarro:%d",xbee->NIxbeeCarro,xbee->NI,semicarro);
	
return ret;	
}
/*
 parametro: dopo quanto tempo
 ritorna timestamp al tempo settato

*/

unsigned int setTimer(unsigned int tempo)
{
unsigned int msec;

  struct timeval start;
   gettimeofday (&start, NULL);
   msec= (start.tv_sec * 1000) + (start.tv_usec / 1000);
   
   msec=msec+tempo;
   return msec;
}




/*
* fa parte della gestione_Connessione_Master 
*
*/

int assegna_slave_carro(xbee_hnd *xbee,xbee_hnd *xbeevicino)
{
 char assegnaNI[13];
 char strcarro[1];

 char risposta[9];
 int ret=1,nbytes=0;
 char tibattezzo[18];
 char buffer[100];


 memset(&assegnaNI,0,sizeof(assegnaNI));
 memset(&strcarro,0,sizeof(strcarro));
 memset(&risposta,0,sizeof(risposta));
 memset(&xbee->NI,0,sizeof(xbee->NI));
  memset(&(xbee->modotrasferimento),0,sizeof(xbee->modotrasferimento));


/*ricevo il modo di trasferimento */	  
 memset(buffer,0,sizeof(buffer));	
	   memset(tibattezzo,0,sizeof(tibattezzo));
 memcpy(tibattezzo,"BATT-OK|",8);
 memcpy(tibattezzo+8,xbeevicino->NI,7);
 
 
 if((nbytes = Readlinetime(xbeevicino->ttyfd, buffer, 19,xbeevicino->timeout)) > 0)
	{
             err_msg ("sono arrivati %d dati %s",nbytes,buffer);
             
                if((memcmp(buffer,tibattezzo,15) == 0))
			          {
			          	//"BATT-OK|MASTE-X-X\n"
			          	//la seconda X e' il modotrasferimento U-up D-down F-fullduplex
			          	 memcpy(&(xbeevicino->modotrasferimento),buffer+16,1);
			          	// err_msg("buffer=%s xbee->modotrasferimento=%c",buffer,xbee->modotrasferimento);
			          }
 
 
 
 
 
	    
	  err_msg ("xbeevicino->carro %d",xbeevicino->carro);
	  err_msg ("xbeevicino->NI %s",xbeevicino->NI);

	  
	  sprintf(strcarro,"%d",xbeevicino->carro);
	   memcpy(assegnaNI,"NISLAVE-",8);
	    strncat(assegnaNI,strcarro,1);
	  
	  

 
	   //lo carico nella struttura
      memcpy(xbee->NI,"SLAVE-",6);
      memcpy((xbee->NI)+6,strcarro,1);
      xbee->modotrasferimento=xbeevicino->modotrasferimento;
      
       err_msg ("xbee->NI %s",xbee->NI);
        err_msg ("assegnaNI %s",assegnaNI);
if(xbee->modoAT==0)//check se non è attivo modo AT
{
	if((init_modem(xbee))==0) 
	{
	 err_msg ("Modem inizializzato ");
	 xbee->modoAT=1;
	}
	else
	{
	  err_msg ("ERRORE Modem non inizializzato");
	}
}  
 
 if(xbee->modoAT==1)
 	{          
	   if((commandAT(xbee,assegnaNI,risposta))==0)  // mi assegno il nome
	   {
	        err_msg ("comando %s eseguito %s",assegnaNI,risposta);
	        
	       memcpy(xbee->NIxbeeCarro,xbeevicino->NI,sizeof(xbee->NIxbeeCarro));
	       memcpy(xbeevicino->NIxbeeCarro,xbee->NI,sizeof(xbeevicino->NIxbeeCarro));
	       xbee->carro=(unsigned char)(xbeevicino->carro);
	       ret=0;
	       xbeevicino->mode=REGIME;
	       xbeevicino->timeout=0;
	        
	   }
	   else
	   {
	         err_msg ("comando %s fallito %s",assegnaNI,risposta);
	   }
  }
  
 	if(xbee->modoAT==1)
		{
		  	 if((release_modem(xbee))==0) 
			{
			 err_msg ("Modem rilasciato, modalita' trasparente abilitata:");
			  xbee->modoAT=0;
			}
			else
			{
			  err_msg ("ERRORE Modem non Rilasciato");
			}

		}
		
		
		
}		

	return ret;

}

/*
* in modalità normale trasmette soltanto in trasparent mode
* dopo aver trovato lo slave piu vicino ed essersi nominato master, per instaurare il link bidirezionale attende
  la conferma dallo slave del carro in cui si connette, e quando arriva battezza anche come slave l'altro xbee dello
  stesso carro
* la stringa che attende sarà  BATT-OK|[NI]-[M]
* ret=0 link attivato
*/

int gestione_Connessione_Master(xbee_hnd *xbee,int dimensione)
{
char tibattezzo[18];
int ret=1,nbytes;
char buffer[255];
static int contatore=0;


 memset(tibattezzo,0,sizeof(tibattezzo));
 memcpy(tibattezzo,"BATT-OK|",8);
 memcpy(tibattezzo+8,xbee->NI,7);

 memset(buffer,0,sizeof(buffer));	
   err_msg ("Attendo dati da %s",(xbee->connesso).NI); 
 //err_msg ("tibattezzo=%s",tibattezzo);
	

	if((nbytes = Readlinetime(xbee->ttyfd, buffer, dimensione,xbee->timeout)) > 0)
	{
             err_msg ("sono arrivati %d dati %s",nbytes,buffer);
             
                if((memcmp(buffer,tibattezzo,15) == 0))
			          {
			          	contatore=0;
			          	//"BATT-OK|MASTE-X-X\n"
			          	//la seconda X e' il modotrasferimento U-up D-down F-fullduplex
			          	 memcpy(&(xbee->modotrasferimento),buffer+16,1);
			          	// err_msg("buffer=%s xbee->modotrasferimento=%c",buffer,xbee->modotrasferimento);
			          }
     
                          err_msg ("link attivato");
                          xbee->link=1;
                          ret=0;
                          xbee->mode=REGIME;
                          xbee->timeout=0;
	
	 	
	}
	else
	{
	   ret=1;
	   contatore++;
	   
	   if(contatore>10)
	   	{
	      xbee->mode=ABILITAZIONE;
	      contatore=0; 
	     }
	    if(tcflush(xbee->ttyfd, TCIOFLUSH)==-1)
				{
					   err_sys ("(%s) tcflush()", prog);
				
				}
	}
	
	
		if(xbee->mode==ABILITAZIONE)
	   {
	   	
	   	 if(battesimo(xbee)==0)
			 {
			   
			   xbee->mode=ATTESA;
						
			 }
	   	
	  }		

//sleep(15);
 return ret;

}





int gestioneMaster(xbee_hnd *xbee,int dimensione,char *buffETHTX)
{

//int nbytes=0,ret=0;
char buffer[100];
int nbytes=0,ret=1;
char risposta[70];


 memset(xbee->bufferRX,0,sizeof(xbee->bufferRX));	
 memset(buffer,0,sizeof(buffer));	
  memset(risposta,0,sizeof(risposta));	
   err_msg ("Attendo conferma da %s",(xbee->connesso).NI); 
   
   err_msg ("Timeout %ld",xbee->timeout);
   if((nbytes = Readlinetime(xbee->ttyfd, buffer, dimensione,(long)xbee->timeout)) > 0)
	{
				
             err_msg ("sono arrivati %d dati %s",nbytes,buffer);
         ret=0;     
         
            
      				
				 if((memcmp(buffer+42,"SONO-UN-PACCHETTO-",18) == 0))
							{
								  memcpy(xbee->bufferRX,buffer,nbytes);//scrivo il buffer di invio su ETHERNET        

			        xbee->timeout=0;
							}
							else
								xbee->timeout=1;
								
				    /*
          entra nell'IF se:
          	c'è qualcosa nel buffer e nello stesso tempo 
          	o tocca a lui o da eth non è arrivato niente
          	lockwrite: implementa un DRR - deficit round robin (con deficit value counter =0)
         */
         

         
		   	if((*(xbee->bufferTX)!=0)&&(((xbee->lockwrite)>0)||(*(buffETHTX)==0)))
		   		{
							 memcpy(risposta,xbee->bufferTX,sizeof(risposta));
							 xbee->lockwrite=0;
						   memset(xbee->bufferTX,0,sizeof(xbee->bufferTX));							  
							  
					}
				else
					{	
					   memcpy(risposta,"Lorem ipsum dolor sit amet, consectetuer adipiscing elit.Aenean ferme\n",70);       
    			   
    			   if(*(buffETHTX)!=0)
				   		{
									// memcpy(risposta,buffETHTX,strlen(buffETHTX));
									memcpy(risposta,buffETHTX,sizeof(risposta));
							}
    			  
    			  xbee->lockwrite=1;	
    			
    			}
							
					  
			
	 
	}
	else
	{
	  ret=1;
				 
	   
	}
   /*
     if(tcflush(xbee->ttyfd, TCIOFLUSH)==-1)
				{
					   err_msg ("(%s) tcflush()", prog);
				
				}	*/
				
		
   Writen(xbee->ttyfd,risposta, dimensione-1); 
   
//     *(risposta+70)='\0';					
  err_msg ("TX dim:%d %s",sizeof(risposta),risposta);
 /*  for (tries = 0; tries < 3; tries ++)
											{
												  
												    // if(write(xbee->ttyfd,rispostaBATT, strlen(rispostaBATT)) < strlen(rispostaBATT))
												  if(write(xbee->ttyfd,risposta, strlen(risposta)) < strlen(risposta))
													{
														err_msg ("Riinvio...");
														continue;
												 	}else
												 	{
													err_msg ("Messaggio=%s di %d byte inviato ...",risposta,strlen(risposta));

													}
								
												     break;
								       }*/
   
   
   
 return ret;  	

}

/*
 si preoccupa della connessione con il master del prossimo carro
 * attende  BATTESIMO|[NI]
 * risponde BATT-OK|[NI]-[M]
*/

int gestione_Connessione_Slave(xbee_hnd *xbee,int dimensione)
{
 int nbytes=0,ret=-1,tries;
char buffer[25];
char NILink[8];
char selectNode[13];
char rispostaBATT[18];
char linefield[1]={'\n'};

 memset(buffer,0,sizeof(buffer));
 memset(NILink,0,sizeof(NILink));
 memset(selectNode,0,sizeof(selectNode));


 // err_msg ("Attendo dati da %s",(xbee->connesso).NI); 
   
 //write(xbee2->ttyfd, "\r", 140);

if(xbee->mode==CONNESSIONE)
{

	if((nbytes = Readlinetime(xbee->ttyfd, buffer, dimensione,xbee->timeout)) > 0)
//if ((nbytes = read(xbee->ttyfd, buffer, 18)) > 0)
	{
             err_msg ("sono arrivati %d dati %s",nbytes,buffer);
             
       if((memcmp(buffer,"BATTESIMO",9) == 0))
			{
                      //   sscanf(buffer,"%s|%s",Inseriscipath);
                         
                          memcpy(NILink,buffer+10,8);
                          memcpy(selectNode,"DN\0",3);
                          NILink[7]=0;
                          strncat(selectNode,NILink,strlen(NILink));
                          
                          err_msg ("Comando seleziona Master %s",selectNode);
               
					    if(xbee->modoAT==0)//check se non è attivo modo AT
							{
								if((init_modem(xbee))==0) 
								{
								 err_msg ("Modem inizializzato ");
								 xbee->modoAT=1;
								}
								else
								{
								  err_msg ("ERRORE Modem non inizializzato");
								}
							} 
                
				      if(xbee->modoAT==1) 
							{
					        sleep(2);                    
				         if((commandAT(xbee,selectNode,NULL))==0)  // mi assegno il nome
							   {
							   	
							   	
	
							   	 memcpy((xbee->connesso).NI,NILink,7);
							   	 xbee->modoAT=0; // sono in trasparent mode
							   	 
							   	 		xbee->mode=REGIME;
											xbee->link=1;
											xbee->timeout=0;
													ret=0;
			
							   }
							   else
							   {

								         err_msg ("comando %s fallito",selectNode);
							   }
        
									
								
								
							if(xbee->modoAT==1)
								{
								  	 if((release_modem(xbee))==0) 
									{
									 err_msg ("Modem rilasciato, modalita' trasparente abilitata:");
									  xbee->modoAT=0;
									}
									else
									{
									  err_msg ("ERRORE Modem non Rilasciato");
									}
				
								}
									
							}
							else
							{
							  err_msg ("ERRORE Modem non inizializzato\n");
							} 			
		
					
                       
}
			else
			  err_msg ("NON RICEVO IL GIUSTO MESS:%s|FINE",buffer);
	 
	}
	else
	{
	
	   err_msg ("Errore (%d),buffer=%s",nbytes,buffer); 
	   ret=1;
	    if(tcflush(xbee->ttyfd, TCIOFLUSH)==-1)
				{
					   err_sys ("(%s) tcflush()", prog);
				
				}
	   
	     
	}
	
 
}

if(xbee->mode==ATTESA)
{
	err_msg ("ATTESA");
	

	if((nbytes = Readlinetime(xbee->ttyfd, buffer, dimensione,xbee->timeout)) > 0)
	{
             err_msg ("sono arrivati %d dati %s",nbytes,buffer);
             
      if((memcmp(buffer,"BATTESIMO",9) == 0))
			{
				xbee->mode=REGIME;
					xbee->link=1;
					xbee->timeout=0;
					ret=0;
			}
			else
				{
					  
			  err_msg ("NON RICEVO IL GIUSTO MESS:%s|FINE",buffer);
			  
			}
	 
	}
	else
	{
	  ret=1;
	   
	}
	
	
	
}

 
 if(xbee->mode==REGIME)
 {
 	
 	   memset(rispostaBATT,0,sizeof(rispostaBATT));	
                    memcpy(rispostaBATT,"BATT-OK|",8);
							      memcpy(rispostaBATT+8,(xbee->connesso).NI,7);
							      
							      memcpy(rispostaBATT+15,"-",1);
							      memcpy(rispostaBATT+16,&(xbee->modotrasferimento),1);     
							     // memcpy(rispostaBATT+15,"-U",2); /*lo slave da il proprio modotrasferimento U-up D-down F-fullduplex*/
							      memcpy(rispostaBATT+17,linefield,1);
							      
 	  info_msg("Messaggio=%s di %d byte",rispostaBATT,strlen(rispostaBATT));
 	
       for (tries = 0; tries < 3; tries ++)
											{
												       /* send an AT command followed by a CR */
												    // 
												//  if(write(xbee->ttyfd,"BATT-OK|MASTE-1-U\n", 18) < 18)
												if(write(xbee->ttyfd,rispostaBATT, 18) < 18)
													{
														err_msg ("Riinvio...");
														continue;
												 	}else
												 	{
													err_msg ("Messaggio=%s di %d byte inviato ...",rispostaBATT,strlen(rispostaBATT));
											
													
													ret=0;
													}
													
													break;
                      }
 
 }
 return ret;

}

/*
* in modalità normale riceve soltanto in trasparent mode
* viene settato dal carro come slave in command mode fase connesione
*
*/
int gestioneSlave(xbee_hnd *xbee,int dimensione,char *buffETHTX)
{

int nbytes=0,ret=1;
char buffer[100];
char risposta[70];



 memset(buffer,0,sizeof(buffer));	
  memset(risposta,0,sizeof(risposta));		
  memset((xbee->bufferRX),0,sizeof(xbee->bufferRX));	
   err_msg ("Attendo dati da %s",(xbee->connesso).NI); 
//write(xbee2->ttyfd, "\r", 140);
	

	if((nbytes = Readlinetime(xbee->ttyfd, buffer, dimensione,xbee->timeout)) > 0)
	{
		ret=0;
             
          err_msg ("sono arrivati %d dati %s",nbytes,buffer);   
          
	   if((memcmp(buffer+42,"SONO-UN-PACCHETTO-",18) == 0))
				{
					  memcpy(xbee->bufferRX,buffer,nbytes);//scrivo il buffer di invio su ETHERNET        
		  
				  xbee->timeout=0;
				}
							else
								xbee->timeout=1;
         
         /*
          entra nell'IF se:
          	c'è qualcosa nel buffer e nello stesso tempo 
          	o tocca a lui o da eth non è arrivato niente
          	lockwrite: implementa un DRR - deficit round robin (con deficit value counter =0)
         */

         
		   	if((*(xbee->bufferTX)!=0)&&(((xbee->lockwrite)>0)||(*(buffETHTX)==0)))
		   		{
							 memcpy(risposta,xbee->bufferTX,sizeof(risposta));
							 xbee->lockwrite=0;	
							 memset(xbee->bufferTX,0,sizeof(xbee->bufferTX));			
//							   memset(xbee->bufferTX,0,strlen(xbee->bufferTX));
             
					}
				else
					{	
					   memcpy(risposta,"Lorem ipsum dolor sit amet, consectetuer adipiscing elit.Aenean ferme\n",70);       
    			   
    			   if(*(buffETHTX)!=0)
				   		{
									// memcpy(risposta,buffETHTX,strlen(buffETHTX));
									 memcpy(risposta,buffETHTX,sizeof(risposta));
							}
    			  
    			  xbee->lockwrite=1;	
    			
    			}
					
	}
	else
	{
		
		 memcpy(risposta,"QUESTA E'LA PRIMA\n",18);
	
	   err_msg ("Errore Ricezione (%d) di RX=%s da TX=%s",nbytes,xbee->NI,(xbee->connesso).NI); 
	   ret=1;
	   
	   
	}
	
/*
	      if(tcflush(xbee->ttyfd, TCIOFLUSH)==-1)
				{
					   err_msg ("(%s) tcflush()", prog);
				
				}	*/

 Writen (xbee->ttyfd,risposta, dimensione -1); 	
 	//     *(risposta+70)='\0';	
 err_msg ("TX dim:%d %s",sizeof(risposta),risposta);			
  /* for (tries = 0; tries < 3; tries ++)
			{
			
				    // if(write(xbee->ttyfd,rispostaBATT, strlen(rispostaBATT)) < strlen(rispostaBATT))
				  if(write(xbee->ttyfd,risposta, strlen(risposta)) < strlen(risposta))
					{
						err_msg ("Riinvio...");
						continue;
				 	}else
				 	{
						err_msg ("Messaggio=%s di %d byte inviato ...",risposta,strlen(risposta));
						
					}

				     break;
       }*/
       
       
	
	
return ret;

}


int scanning(xbee_hnd *xbee,const char* comando)
{
      char buffer[255];  /* Input buffer */
    
      int  nbytes=0,dim;       /* Number of bytes read */
      int  n,tries,ret=-1,indice=0;        /* Number of tries so far */
      char vmin=0,vtime=0;
int arrivato=0,nodiscoperti=0;
     char cmdfinale[13]="AT",cmdfinaleW[13];



vmin=(char)(xbee->status).c_cc[VMIN]; 
vtime=(char)(xbee->status).c_cc[VTIME];

(xbee->status).c_cc[VMIN]  = 4; 
(xbee->status).c_cc[VTIME] = 5;

if(tcflush(xbee->ttyfd, TCIOFLUSH)==-1)
	{
		   err_msg ("(%s) tcflush()", prog);
		return -1;
	}	
	if(tcsetattr(xbee->ttyfd, TCSANOW, &(xbee->status))==-1) 
	{
		   err_msg ("(%s) tcsetattr()", prog);
		return -1;
	} 

if(xbee->modoAT==0)//check se non è attivo modo AT
{
	if((init_modem(xbee))==0) 
	{
	 err_msg ("Modem inizializzato ");
	 xbee->modoAT=1;
	}
	else
	{
	  err_msg ("ERRORE Modem non inizializzato");
	}
}


 strncat(cmdfinale,comando,strlen(comando));
// err_msg ("cmdfinale (%s) ", cmdfinale);
 memcpy(cmdfinaleW,cmdfinale,13);
 strncat(cmdfinaleW,"\r",1);

 dim=strlen(cmdfinaleW);


      for (tries = 0; tries < 3; tries ++)
      {
       /* send an AT command followed by a CR */
     if(write(xbee->ttyfd,cmdfinaleW, dim) < dim)
	{
		err_msg ("Riinvio...");
		continue;
 	}else
	err_msg ("Comando=%s di %d byte inviato ...",cmdfinale,dim);


     break;

        }



 memset(buffer,0,sizeof(buffer));
    while((n=Readlinetime(xbee->ttyfd,buffer,10,xbee->timeout))>0)
	{
	  //err_msg ("buffer:%s DIMENSIONE=%d",buffer,n);
	  
	  
	  if((memcmp(buffer,"ERROR", 5) == 0))
			{
				ret=2; // è arrivato ERROR
				break;
			}
         
		if ((strncmp(buffer, "\n", 1) == 0)&&!arrivato)
		{
		// err_msg ("LF");
		 arrivato=1;
                 nbytes=0;
		 
                if(indice==0) // la prima entrata
		   continue;
                // err_msg ("NODO:%s",xbee->nodediscovery);
		 indice=0;

                 
		 memcpy(&(xbee->nodi[nodiscoperti]),&(xbee->nodojoin),sizeof(xbee_nodo));
              //   err_msg ("NODO INSERITO:%s",xbee->nodi[nodiscoperti].NI);
                 nodiscoperti++;
                 xbee->nr_nodiscoperti=nodiscoperti;
		if(nodiscoperti>=MAXNODIDISCOVERY)
		{
 		  err_msg ("Raggiunto il numero Massimo di Nodi Scoperti pari a:%d",MAXNODIDISCOVERY);;
                              
				ret=1;
				break;
		}
		         /*if((memcmp((xbee->nodojoin).NI, "COORD", 5) == 0))
			{
                                err_msg ("NI:%s",(xbee->nodojoin).NI);
                              
				ret=0;
				break;
			}*/
		
		}
		else if((strncmp(buffer, "\n", 1) == 0)&& arrivato)
		{
		  // doppio LF ricevuto 	
                  ret=1;
		   err_msg ("NODI MEMORIZZATI:%d",nodiscoperti);
		   err_msg ("FINE SCANNER%s",buffer);
		  break;
		}
		else
		{
                   ret=0;
                  memcpy(&(xbee->nodediscovery[nbytes]),buffer,n);
		  xbee->nodediscovery[nbytes+n-1]='\0';
                  nbytes = nbytes +DimNodoMio[indice];
		  
 		 indice++;
			arrivato=0;	
                   
		}
	}

if(n<0)
{
   err_msg ("(%s)Readlinetime()", prog);
 ret=-1;
 }    
      

(xbee->status).c_cc[VMIN]  = vmin; 
(xbee->status).c_cc[VTIME] = vtime;

if(tcflush(xbee->ttyfd, TCIOFLUSH)==-1)
	{
		   err_msg ("(%s) tcflush()", prog);
		ret=-1;
	}	
	if(tcsetattr(xbee->ttyfd, TCSANOW, &(xbee->status))==-1) 
	{
		   err_msg ("(%s) tcsetattr()", prog);
		ret=-1;
	} 
  
  
   
      return ret;

 
}


/*
 la funzione deve essere chiamata dopo aver effettuato uno scanning
 * ret = 1 non ha trovato un buon segnale
 * ret = 0 segnale ok trovato
*/

int check_and_select(xbee_hnd *xbee)
{
int i,ris,ret=1,free=0; 
char risposta[9];
char NI[8];

xbee_nodo nodok;
 char NDspecifico[13];
 char selectNode[13];
 char assegnaNI[13];
unsigned char RSSI=0;
unsigned char Ncarro=0;

unsigned char NumMaxSlaveCurr=0;
unsigned char NumMaxSlavePrev=0;
char strcarro[1];


if(xbee->modoAT==0)//check se non è attivo modo AT
{
	if((init_modem(xbee))==0) 
	{
	 err_msg ("Modem inizializzato ");
	 xbee->modoAT=1;
	}
	else
	{
	  err_msg ("ERRORE Modem non inizializzato");
	}
}

 memset(&nodok,0,sizeof(nodok));
 memset(&risposta,0,sizeof(risposta));
 memset(&NI,0,sizeof(NI));
 memset(&assegnaNI,0,sizeof(assegnaNI));
 memset(&selectNode,0,sizeof(selectNode));
  
 

/* memorizzo l nome degli NI trovati liberi */
for(i=0,free=0;i< xbee->nr_nodiscoperti;i++)
{

		if((memcmp((xbee->nodi[i]).NI, "SLAVE", 5) == 0))
			{
					  NumMaxSlaveCurr=(unsigned char)strtol((xbee->nodi[i]).NI+6, (char**)NULL, 10);
                       if(NumMaxSlaveCurr >= NumMaxSlavePrev)
                       {
                       	  NumMaxSlavePrev=NumMaxSlaveCurr;
                          memcpy(NI,(xbee->nodi[i]).NI,8);
                          memcpy(&(nodok),&(xbee->nodi[i]),sizeof(xbee_nodo));
                          err_msg ("NI libero %s", NI);
                        
                          
                          
                        }
                        else
                        	err_msg ("NI NumMaxSlaveCurr(%d) < NumMaxSlavePrev(%d)", NumMaxSlaveCurr,NumMaxSlavePrev);
			
			
			free=1;	//continua solo se trova lo SLAVE
			}

 

}
	

for(i=0;i< free ;i++)
{
 
    memcpy(NDspecifico,"ND\0",3);
    memcpy(selectNode,"DN\0",3);
// setto RSSI a '0' ATDB0
/*
	  if((ris=commandAT(xbee,"DB0",risposta))==0) 
	{
	 err_msg ("comando DB0 eseguito %s",risposta);
	}
	else
	{
	  err_msg ("comando DB0 fallito %s",risposta);
	}
*/
// preparo la stringa ATNDDEFAULT\r
	strncat(NDspecifico,NI,strlen(NI));
	/* inserire il node discovery del primo NI trovato*/

	if(scanning(xbee,NDspecifico)!=1)
			break;

//	 err_msg ("NDspecifico %s",NDspecifico);

// leggo RSSI 

	  if((ris=commandAT(xbee,"DB",risposta))==0) 
	{
	 err_msg ("comando DB eseguito %s",risposta);
	}
	else
	{
	  err_msg ("comando DB fallito %s",risposta);
	}


       RSSI=(unsigned char)strtol(risposta, (char**)NULL, 16);

// controllo io valore dell'RSSI 

 // err_msg ("RSSI 0x%02x",RSSI);
  
   if((RSSI>=0x01) && (RSSI<=0x35))
	{
	    
	    //mi assegno il nome di master
	 NI[7]=0;
	 Ncarro=(unsigned char)strtol(NI+6, (char**)NULL, 10);
	  
	  
	  
	  Ncarro++;     // definisco la numerazione del carro in cui sto  
	  xbee->carro=(unsigned char)Ncarro;
	 // err_msg ("Ncarro %d",Ncarro);
	 
	   // converti da int a string
	  sprintf(strcarro,"%d",Ncarro);
	  err_msg ("strcarro %s",strcarro);
	  memcpy(assegnaNI,"NIMASTE-",8);
	 // err_msg ("assegnaNI %s",assegnaNI);
	  strncat(assegnaNI,strcarro,1); 
        
         //lo carico nella struttura
         memset(&xbee->NI,0,sizeof(xbee->NI));
           memcpy(xbee->NI,"MASTE-",6);
           strncat(xbee->NI,strcarro,1); 
         
    //        err_msg ("xbee->NI %s",xbee->NI);
      //      err_msg ("assegnaNI %s",assegnaNI);
         
	   if((ris=commandAT(xbee,assegnaNI,risposta))==0)  // mi assegno il nome
	   {
	        err_msg ("comando %s eseguito %s",assegnaNI,risposta);
	   }
	   else
	   {
	         err_msg ("comando %s fallito %s",assegnaNI,risposta);
	   }
	    
	     strncat(selectNode,NI,strlen(NI));
	     
	     // invio comando DN per ottenere l'indirizzo dello slave
	     
			if((ris=commandAT(xbee,selectNode,risposta))==0) 
			{
			 err_msg ("comando %s eseguito %s",selectNode,risposta);
			 // invio la stringa per battezzare il nodo a cui mi collego
			 xbee->modoAT=0;
			 sleep(5);
			 if((ret=battesimo(xbee))==0)
			 {
			   xbee->connesso= nodok;
			   			xbee->mode=ATTESA;
			  break;
			 }
			
			
			}
			else
			{
			  err_msg ("comando %s fallito %s",selectNode,risposta);
			  	    ret=1;
			}
	
	}
}

				if(xbee->modoAT==1)
					{
					  if((release_modem(xbee))==0) 
						{
						 err_msg ("Modem rilasciato, modalita' trasparente abilitata:");
						  xbee->modoAT=0;
						}
						else
						{
						  err_msg ("ERRORE Modem non Rilasciato");
						}
	
					}

 return ret;
}



int battesimo(xbee_hnd *xbee)
{

// formato= BATTESIMO|[NI]
char tibattezzo[18];
int tries;
int ret=1;



 memset(tibattezzo,0,sizeof(tibattezzo));
 memcpy(tibattezzo,"BATTESIMO|",10);
//tibattezzo[17]='\n';
//err_msg ("tibattezzo =%s di %d byte ",tibattezzo,sizeof(tibattezzo));
//err_msg ("xbee->NI=%s di %d byte ",xbee->NI,sizeof(xbee->NI));
 strncat(tibattezzo,xbee->NI,7); 
 memcpy(tibattezzo+17,"\n",1);


for (tries = 0; tries < 3; tries ++)
  {
       /* send an AT command followed by a CR */
     if(write(xbee->ttyfd,tibattezzo, strlen(tibattezzo)) < strlen(tibattezzo))
	{
		err_msg ("Riinvio...");
		continue;
 	}else
 	{
	err_msg ("Messaggio Remoto=%s di %d byte inviato ...",tibattezzo,sizeof(tibattezzo));
	 ret=0;
	// exit(0);
	 break;
	}
  }


 return ret;
}





/*
 *  ret: *risposta = stringa della risposta
 * 	 -1 : comando fallito
 *	  0 : risposta "OK" o valore
 *	  1 : risposta "ERROR"
 */

int commandAT(xbee_hnd *xbee,const char *comando,char *risposta)
{
      char buffer[255];  /* Input buffer */
      int  nbytes=0,dim=0;       /* Number of bytes read */
      int  tries,ret=-1;        /* Number of tries so far */
      char vmin=0,vtime=0;

     char cmdfinale[13]="AT",cmdfinaleW[13];


vmin=(char)(xbee->status).c_cc[VMIN]; 
vtime=(char)(xbee->status).c_cc[VTIME];

(xbee->status).c_cc[VMIN]  = 2; 
(xbee->status).c_cc[VTIME] = 5;

if(tcflush(xbee->ttyfd, TCIOFLUSH)==-1)
	{
		   err_msg ("(%s) tcflush()", prog);
		return -1;
	}	
	if(tcsetattr(xbee->ttyfd, TCSANOW, &(xbee->status))==-1) 
	{
		   err_msg ("(%s) tcsetattr()", prog);
		return -1;
	} 





 strncat(cmdfinale,comando,strlen(comando));

 memcpy(cmdfinaleW,cmdfinale,13);
 strncat(cmdfinaleW,"\r",1);

dim=strlen(cmdfinaleW);
err_msg ("dim %d",dim);
  for (tries = 0; tries < 3; tries ++)
 {
       /* send an AT command followed by a CR */
     if(write(xbee->ttyfd,cmdfinaleW, dim) < dim)
	{
		//err_msg ("Riinvio...");
		continue;
 	}else
	err_msg ("Comando=%s di %d byte inviato ...",cmdfinale,dim);





  if((nbytes = Readlinetime(xbee->ttyfd, buffer, 10,xbee->timeout)) > 0)
	{
     err_msg ("sono arrivati %d dati %s",nbytes,buffer);
	 
	}
	else
	   err_msg ("(%s)Readlinetime()", prog);


       /* nul terminate the string and see if we got an OK response */
	//*bufptr = '\0';

	if (memcmp(buffer, "OK", 2) == 0)
	  ret=0;

 if (memcmp(buffer, "ERROR", 5) == 0)
	  ret=1;

    
       if(risposta!=NULL)
       memcpy(risposta,buffer,nbytes);
       
     break;
  }
  if(tries==4)
   {
	err_msg ("Numero tentativi falliti");
	ret=-1;
   }







(xbee->status).c_cc[VMIN]  = vmin; 
(xbee->status).c_cc[VTIME] = vtime;

if(tcflush(xbee->ttyfd, TCIOFLUSH)==-1)
	{
		   err_msg ("(%s) tcflush()", prog);
		ret=-1;
	}	
	if(tcsetattr(xbee->ttyfd, TCSANOW, &(xbee->status))==-1) 
	{
		   err_msg ("(%s) tcsetattr()", prog);
		ret=-1;
	} 
     
      return ret;

}
/*
 *isDOWN ritorno vero se il modo trasferimento e' DOWN
 *
 */ 

int isDOWN(xbee_hnd *xbee)
{
	int ret=0;
	
	if((memcmp(&(xbee->modotrasferimento),"D",1) == 0))
	   ret=1;
	
	return ret;
}

/*
 *isUP ritorno vero se il modo trasferimento e' UP
 *
 */ 

int isUP(xbee_hnd *xbee)
{
	int ret=0;
	
	if((memcmp(&(xbee->modotrasferimento),"U",1) == 0))
	   ret=1;
	
	return ret;
}

/*

 if(tcflush(xbee->ttyfd, TCIOFLUSH)==-1)
				{
					   err_sys ("(%s) tcflush()", prog);
				
				}	
				if(tcsetattr(xbee->ttyfd, TCSANOW, &(xbee->status))==-1) 
				{
					   err_sys ("(%s) tcsetattr()", prog);
				
				} 
				
*/
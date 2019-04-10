

int scanning(xbee_hnd *xbee,const char* comando);
int commandAT(xbee_hnd *xbee,const char *comando,char *risposta);
int check_and_select(xbee_hnd *xbee);
int battesimo(xbee_hnd *xbee);
int Config_MasterSlave(xbee_hnd *xbee,int semicarro);
 int release_modem(xbee_hnd *xbee);
  int init_modem(xbee_hnd *xbee);
  xbee_hnd *xbee_setup(char *path, int baudrate);
  
  int gestione_Connessione_Master(xbee_hnd *xbee,int dimensione);
  int assegna_slave_carro(xbee_hnd *xbee,xbee_hnd *xbeevicino);

  int gestioneMaster(xbee_hnd *xbee,int dimensione,char *buffETHTX);
  int gestione_Connessione_Slave(xbee_hnd *xbee,int dimensione);
  int gestioneSlave(xbee_hnd *xbee,int dimensione,char *buffETHTX);
  unsigned int setTimer(unsigned int tempo);
  int isDOWN(xbee_hnd *xbee);
  int isUP(xbee_hnd *xbee);
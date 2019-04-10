#include <stdio.h>
#include <sys/time.h>

int main()
{
struct timeval start;
  struct tm* ptm;
  long milliseconds=0;
  char time_string[40];
  
  int prova=0;
  

/*
printf("|char | = %d byte\n",sizeof(char));

printf("|short | = %d byte\n",sizeof(short));

printf("|unsigned short | = %d byte\n",sizeof(unsigned short));

printf("|unsigned char | = %d byte\n",sizeof(unsigned char));

printf("|int | = %d byte\n",sizeof(int));

printf("|unsigned int | = %d byte\n",sizeof(unsigned int));

printf("|long | = %d byte\n",sizeof(long));

printf("|unsigned long | = %d byte\n",sizeof(unsigned long));

printf("|void* | = %d byte\n",sizeof(void*));

printf("|struct timeval | = %d byte\n",sizeof(struct timeval));
 
printf("|long int | = %d byte\n",sizeof(long int));

printf("|time_t | = %d byte\n",sizeof(time_t));
*/



printf("QUESTA CLASSE e' AL PRIMO ANNO e su tutti bigghiola\n");

while(1)
{
prova++;
//sleep(10);



   gettimeofday (&start, NULL);
  ptm = localtime (&start.tv_sec);
  	strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
milliseconds=start.tv_usec;
//printf("%ld %ld\n", start.tv_sec, start.tv_usec);
//printf("%s.%06ld\n", time_string, milliseconds);


 
}



return 0;

}

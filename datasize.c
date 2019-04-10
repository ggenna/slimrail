#include <stdio.h>

int main()
{
printf("|char | = %d byte\n",sizeof(char));

printf("|unsigned char | = %u byte\n",sizeof(unsigned char));

printf("|int | = %d byte\n",sizeof(int));

printf("|long | = %d byte\n",sizeof(long));

printf("|void* | = %d byte\n",sizeof(void*));

return 0;

}

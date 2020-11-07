#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <time.h>

int flag=1, conta=1;

void atende()                   // atende alarme
{
	printf("alarme # %d\n", conta);
	flag=1;
	conta++;
}


main()
{

(void) signal(SIGALRM, atende);  // instala  rotina que atende interrupcao

while(conta < 4){
   if(flag){
      alarm(3);                 // activa alarme de 3s
      flag=0;
   }
}
printf("Vou terminar.\n");

}
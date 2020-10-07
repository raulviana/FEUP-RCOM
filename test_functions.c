#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <strings.h>


int flag=1, conta=1;
void atende()       // atende alarme
{
	if(conta <= 3){
    printf("#%d: Return message not received, waiting 3 more seconds..\n", conta);
    flag=1;
    conta++;
  }
  else{
    printf("[EXITING]\n Remote App couldnt establish communication, aborting\n");
    exit(-1);
  }
  alarm(3);
}

int main(){

     (void) signal(SIGALRM, atende);

alarm(3);
while(1){}
    return 0;
}
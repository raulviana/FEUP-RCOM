
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


#include "alarm.h"
#include "constants.h"
#include "link_layer.h"

int conta = 1;

enum phase link_phase;

void setAlarm(int seconds){
    alarm(seconds);
}

void cancelAlarm(){
    alarm(0);
    conta = 1;
}

void atende()       // atende alarme
{
	if(conta <= MAX_TRIES){
		switch(link_phase){
			case OPENING_CONNECTION:
			    printf("#%d: Return message not received, waiting 3 more seconds..\n", conta);
				sendControl();    
				conta++;
				break;
			case OPEN_CONTROL_PACKET:
				break;
			default:
				break;
		}
  	}
  	else{
    	printf("[EXITING]\n Max timeout reached, aborting\n");
    	exit(-1);
  	}
  	setAlarm(TIMEOUT);
}

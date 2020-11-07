
   #include <stdlib.h>

#include "alarm.h"
#include "constants.h"
#include "link_layer.h"


enum phase link_phase;

void setAlarm(int seconds){
	struct sigaction sa;
	sa.sa_handler = &atende;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	sigaction(SIGALRM, &sa, NULL);
    alarm(seconds);
}

void cancelAlarm(){
	struct sigaction action;
	action.sa_handler = NULL;

	sigaction(SIGALRM, &action, NULL);
    alarm(0);
	continueFlag = FALSE;
	numTries = 1;
}

void atende(int signal)       // atende alarme
{
	if(signal != SIGALRM) return;

	if(numTries <= MAX_TRIES){
		switch(link_phase){
			case OPENING_CONNECTION:
			    printf("[TIMEOUT]\n  #%d: Return message not received\n", numTries);
				sendControl();    
				break;
			case SENDING_DATA:
				printf("[TIMEOUT]\n  #%d: No response packet\n", numTries);
				printf("tries: %d\n", numTries);
				numTries++;
				continueFlag = TRUE;
			
				printf("flag: %d\n", continueFlag);
			
				break;
			case CLOSING_CONNECTION:
				printf("[TIMEOUT]\n  #d: No DISC received\n");
		
			default:
				break;
		}
  	}
  	else{
    	printf("[EXITING]\n  Reached max timeout tries, aborting\n");
    	exit(5);
  	}
  	setAlarm(TIMEOUT);
}

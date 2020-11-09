



#include "alarm.h"



extern Link_control link_control;
extern int receive_id;

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
			    printf("[TIMEOUT]\n  #%d: Return message not received\n", conta);
				sendControl();    
				conta++;
				break;
			case SENDING_DATA:
				printf("[TIMEOUT]\n  #%d: No response packet\n", conta);
				conta++;
				/*if(receive_id == TRUE && link_control.N_s ==1){
					write(fd, REJ1, sizeof(REJ1));
				}
				else if(receive_id == TRUE && link_control.N_s ==0){
					write(fd, REJ0, sizeof(REJ0));
				}*/
				break;
			case CLOSING_CONNECTION:
				printf("[TIMEOUT]\n  #d: No DISC received\n");
				conta++;
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

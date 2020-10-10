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

int main(){

     (void) signal(SIGALRM, atende);

setAlarm(3);
int count = 0;
while(count < 2){
     sleep(7);
     count++;
}
cancelAlarm();
setAlarm(2);
while(1){
     sleep(25);
}

return 0;
}
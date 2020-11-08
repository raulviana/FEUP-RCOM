
   #include <stdlib.h>

#include "alarm.h"
#include "constants.h"
#include "link_layer.h"


enum phase link_phase;

void atende(int signal) {
  if(signal != SIGALRM)
    return;

  printf("Alarm: %d*********************************************\n", numTries + 1);
	continueFlag = 1;
	numTries++;
}

void setAlarm() {
  // set sigaction struct
	struct sigaction sa;
	sa.sa_handler = &atende;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	sigaction(SIGALRM, &sa, NULL);

	continueFlag = 0;

	alarm(TIMEOUT); // install alarm
}

void cancelAlarm() {
  // unset sigaction struct
	struct sigaction action;
	action.sa_handler = NULL;

	sigaction(SIGALRM, &action, NULL);

	alarm(0); // uninstall alarm
}

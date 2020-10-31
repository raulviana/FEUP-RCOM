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

void fun(){
  sleep(4);
}

int main(){

  clock_t t;
  t = clock();
  fun();
  t = clock() - t;
  double taken = ((double) t)  / CLOCKS_PER_SEC;
  printf("time teken: %f\n", taken);
    return 0;
}
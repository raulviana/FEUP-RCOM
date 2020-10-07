


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

#include "constants.h"
#include "link_layer.h"
#include "alarm.h"



struct termios newtio, oldtio;
extern int conta;


int check_bcc1(char control_message[], int size);


int main(int argc, char** argv)
{
  (void) signal(SIGALRM, atende);  // instala  rotina que atende interrupcao

  int fd, c, res;
  int i, sum = 0, speed = 0;
  char buf[255];
  
  // if ( (argc < 2) || 
  //       ((strcmp("/dev/ttyS10", argv[1])!=0) && 
  //       (strcmp("/dev/ttyS11", argv[1])!=0) )) {
  //   printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttySX\n");
  //   exit(1);
  // }
  
  printf("        -->SENDER<--\n");
  
  fd = llopen(SENDER);
  if(fd == -1){
    perror("Termios not properly set\n[EXITING]\n");
    exit(-2);
  }



  char SET[5];
  SET[0] = FLAG;
  SET[1] = CONTROL_A_SC;
  SET[2] = CONTROL_SET;
  SET[3] = BCC(SET[1], SET[2]);
  SET[4] = FLAG;

  /*******************
   * Send Message
   * ****************/
  printf("[STARTING CONNECTION]\n");
  printf("[SENDING MESSAGE]\n");
  printf(" SET: ");
  for (int i = 0; i < 5; i++){  
    printf("%4X ", SET[i]);
  }
  printf("\n");

  res = write(fd, SET, sizeof(SET));
  
  char in_message[255];
  bzero(in_message, sizeof(in_message));
  int position = 0;

  //Recebe mensagem de volta
  int count = 0; 
  setAlarm(3);                 // activa alarme de 3s
  while (count < 5) {    /* loop for input */
    res = read(fd,buf,1);   /* returns after 1 chars have been input */
    printf("debug %d  %4X\n", res, buf[0]);
    in_message[position++] = buf[0];
    count++;
  }
  cancelAlarm();
  printf("[MESSAGE RECEIVED]\n");
  printf(" UA: ");
  for (int i = 0; i < 5; i++){
      printf("%4X ", in_message[i]);
  }
  printf("received\n");

  if(! check_bcc1(in_message, sizeof(in_message))){
    printf("[ERROR]\n BCC check error: exiting!\n");
    exit(-2);
  }
  else{
    printf("[INFO]\n BCC is correct!\n");
  }
  
  printf("[CONNECTION ESTABLISHED]\n");
  
  if(! llclose(fd)) perror("Termios structure not set to its previous state\n");

  return 0;
}


int check_bcc1(char control_message[], int size){
  char this_bcc = control_message[1] ^ control_message[2];
  if(this_bcc == control_message[3]) return TRUE;
  else return FALSE;
}

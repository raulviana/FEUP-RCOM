

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#include "constants.h"
#include "link_layer.h"

extern int conta;

int check_bcc1(char control_message[], int size);

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];
    // if ( (argc < 2) ||
  	//      ((strcmp("/dev/ttyS10", argv[1])!=0) &&
  	//       (strcmp("/dev/ttyS11", argv[1])!=0) )) {
    //   printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    //   exit(1);
    // }

    printf("      -->RECEIVER<--\n");

    fd = llopen(RECEIVER);
    if(fd == -1){
      perror("[ERROR] Could not establish connection\n");
      exit(-1);
    }  //Connection online
    
    char in_message[255];
    bzero(in_message, sizeof(in_message));
    int position = 0;
    int count = 0;

    while (count < 5) {       /* loop for input */
      res = read(fd,buf,1);   /* returns after 1 chars have been input */
      buf[res] = 0;               /* so we can printf... */
      in_message[position++] = buf[0];

      count = count + res;
    }

    printf("[MESSAGE RECEIVED]\n");
    printf(" SET: ");
      for (int i = 0; i < 5; i++){
      printf("%4X ", in_message[i]);
    }
    printf("\n");

    if(! check_bcc1(in_message, sizeof(in_message))){
      printf("[ERROR]\n BCC check error: exiting!\n");
      exit(-2);
    }
    else{
      printf("[INFO]\n BCC is correct!\n");
    }
  
    char UA[5];
    UA[0] = FLAG;
    UA[1] = CONTROL_A_SC;
    UA[2] = CONTROL_UA;
    UA[3] = BCC(UA[1], UA[2]);
    UA[4] = FLAG;

    res = write(fd, UA, sizeof(UA));

    printf("[MESSAGE SENT]\n");
    printf(" UA: ");
    for (int i = 0; i < 5; i++){  
      printf("%4X ", UA[i]);
    }
    printf("\n");

  
    llclose(fd);

    return 0;
}

int check_bcc1(char control_message[], int size){
  char this_bcc = control_message[1] ^ control_message[2];
  if(this_bcc == control_message[3]) return TRUE;
  else return FALSE;
}


#include "link_layer.h"
#include "constants.h"

#include <stdio.h>
#include <termios.h>
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

#define SENDER 1
#define RECEIVER 2

struct termios newtio, oldtio;

int llopen(int type){
    /*
   ***********************************************************************
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
    ************************************************************************
  */
    int fd;

    if(type == SENDER) fd = open(SENDER_PORT, O_RDWR | O_NOCTTY );
    else fd = open(RECEIVER_PORT, O_RDWR | O_NOCTTY );
    
    if (fd <0) {perror(SENDER_PORT); return -1; }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      return -1;
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) pr�ximo(s) caracter(es)
  */
    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      return -1;
    }
    printf("New termios structure is set\n");
/*********************************************/
    return fd;
}

int llclose(int fd){

   //Voltar a colocar a estrutura termios no estado inicial
    sleep(2);
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      return -1;
    }
    close(fd);
    return TRUE;
}
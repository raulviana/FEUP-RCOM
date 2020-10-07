


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


int flag=1, conta=1;
void atende()       // atende alarme
{
	if(conta <= MAX_TRIES){
    printf("#%d: Return message not received, waiting 3 more seconds..\n", conta);
    flag=1;
    conta++;
  }
  else{
    printf("[EXITING]\n Remote App couldnt establish communication, aborting\n");
    exit(-1);
  }
  alarm(TIMEOUT);
}

int check_bcc1(char control_message[], int size);


int main(int argc, char** argv)
{
  (void) signal(SIGALRM, atende);  // instala  rotina que atende interrupcao

  int fd, c, res;
  struct termios oldtio,newtio;
  int i, sum = 0, speed = 0;
  char buf[255];
  
  if ( (argc < 2) || 
        ((strcmp("/dev/ttyS10", argv[1])!=0) && 
        (strcmp("/dev/ttyS11", argv[1])!=0) )) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttySX\n");
    exit(1);
  }
  
  printf("        -->SENDER<--\n");
  /*
   ***********************************************************************
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
    ************************************************************************
  */


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
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
      exit(-1);
    }
    printf("New termios structure is set\n");
/*********************************************/



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
    alarm(3);                 // activa alarme de 3s
    while (count < 5) {       /* loop for input */
      res = read(fd,buf,1);   /* returns after 1 chars have been input */
      in_message[position++] = buf[0];
      count++;
    }
    alarm(0);
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
    
    
  

   //Voltar a colocar a estrutura termios no estado inicial
    sleep(2);
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
    close(fd);
    return 0;
}


int check_bcc1(char control_message[], int size){
  char this_bcc = control_message[1] ^ control_message[2];
  if(this_bcc == control_message[3]) return TRUE;
  else return FALSE;
}

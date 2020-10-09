
#include "link_layer.h"
#include "constants.h"
#include "alarm.h"

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

unsigned char SET[] = { FLAG, CONTROL_A, CONTROL_SET, BCC(CONTROL_A, CONTROL_SET), FLAG};
unsigned char UA[] = {FLAG, CONTROL_A, CONTROL_UA, BCC(CONTROL_A, CONTROL_UA), FLAG};

struct termios newtio, oldtio;
enum state current;

int llopen(int type){

  int fd = startConnection(type);
  int res;
  //sender
  if(type == SENDER){
    //send SET
    printf("[STARTING CONNECTION]\n");
    printf("[SENDING SET]\n");
    res = write(fd, SET, sizeof(SET));
    //receive UA
    char in_message[5];
    unsigned char buf[1];
    bzero(in_message, sizeof(in_message));
    int position = 0;
    int count = 0; 
    setAlarm(3);                 // activa alarme de 3s
    while (count < 5) {    /* loop for input */
      res = read(fd,buf,1);   /* returns after 1 chars have been input */
      in_message[position++] = buf[0];
      count++;
      }
    cancelAlarm();

    if(! check_bcc1(in_message, sizeof(in_message))){
    printf("[ERROR]\n BCC check error: exiting!\n");
    return -1;
    }
    else{
      printf("[INFO]\n BCC is correct!\n");
    }
    }
    //receiver
    else{
      //receive SET
      readMessage(fd, SET);
      printf("[SET RECEIVED]\n");

      //send UA char UA[5];
      UA[0] = FLAG;
      UA[1] = CONTROL_A;
      UA[2] = CONTROL_UA;
      UA[3] = BCC(UA[1], UA[2]);
      UA[4] = FLAG;

      res = write(fd, UA, sizeof(UA));
  
      printf("[UA SENDED]\n");
    }
    

    //OK
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

int startConnection(int type){

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

int readMessage(int fd, unsigned char commandExpected[]){
  current = START;

  unsigned char buf[1];

  while (current != STOP){
    read(fd, buf, 1);
    COM_currentMachine(&current, buf[0]);
  }
}

void COM_currentMachine(enum state* current, unsigned char buf){
  unsigned char control_byte;

  switch (*current){
			case START:
				if (buf==FLAG) *current = READ_FLAG;
        else *current=START;
			break;

			case READ_FLAG:
				if(buf == CONTROL_A) *current = READ_CONTROL_A;
        else if(buf==FLAG)
					*current = READ_FLAG;
				else
					*current= START;
			break;

			case READ_CONTROL_A: 
				if(buf == CONTROL_SET){
          control_byte = buf;
          *current=READ_CONTROL;
        }
				else if(buf==FLAG)
					*current=READ_FLAG;
				else
					*current=START;
			break;

			case READ_CONTROL: 
				if(buf == BCC(CONTROL_A, control_byte)) *current=BCC_OK;
        else if (buf==FLAG) *current=READ_FLAG;
				else
					*current=START;
			break;

			case BCC_OK:
				if(buf==FLAG){
					*current = STOP;
          return;
				}
        else *current=START;
							
			break;

		}	
}

int check_bcc1(char control_message[], int size){
  char this_bcc = control_message[1] ^ control_message[2];
  if(this_bcc == control_message[3]) return TRUE;
  else return FALSE;
}


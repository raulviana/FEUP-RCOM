
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

unsigned char SET[5] = { FLAG, CONTROL, CONTROL_SET, BCC(CONTROL, CONTROL_SET), FLAG};
unsigned char UA[5] = {FLAG, CONTROL, CONTROL_UA, BCC(CONTROL, CONTROL_UA), FLAG};
unsigned char DISC[5] = {FLAG, CONTROL, CONTROL_DISC, BCC(CONTROL, CONTROL_DISC), FLAG};

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
    setAlarm(3);                 // activa alarme de 3s
    readMessage(fd, UA);
    cancelAlarm();
    }
    
    //receiver
    else{
      //receive SET
      readMessage(fd, SET);
      printf("[SET RECEIVED]\n");

      res = write(fd, UA, sizeof(UA));
  
      printf("[UA SENDED]\n");
    }
  
    //OK
    return fd;
}

int llclose(int fd, int type){

  //sender
  if(type == SENDER){
    //send DISC
    printf("[CLOSING CONNECTION]\n [INFO]  Sending DISC\n");
    write(fd, DISC, sizeof(DISC));
    //receive UA
    setAlarm(3);                 // activa alarme de 3s
    readMessage(fd, UA);
    cancelAlarm();
    printf("[INFO]  UA received\n");
  }
  //receiver
  else{
    //receive DISC
    setAlarm(3);                 // activa alarme de 3s
    readMessage(fd, DISC);
    cancelAlarm();
    printf("[INFO]  DISC received\n");
    //send UA
    printf("[INFO]  Sending UA\n");
    write(fd, UA, sizeof(UA));
  }

  if(! closeConnection(fd)) return FALSE;
   
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
  int res;
  while (current != STOP){
    res = read(fd, buf, 1);
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
				if(buf == CONTROL) *current = READ_CONTROL;
        else if(buf==FLAG)
					*current = READ_FLAG;
				else
					*current= START;
			break;

			case READ_CONTROL: 
				if((buf == CONTROL_SET) || (buf == CONTROL_UA) || (buf = CONTROL_DISC)){
          control_byte = buf;
          *current=READ_BCC;
        }
				else if(buf==FLAG)
					*current=READ_FLAG;
				else
					*current=START;
			break;

			case READ_BCC: 
				if(buf == BCC(CONTROL, control_byte)) *current=BCC_OK;
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

int closeConnection(int fd){
  
  //Voltar a colocar a estrutura termios no estado inicial
  sleep(2);
  if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
    perror("tcsetattr");
    return FALSE;
  }
  close(fd);
  return TRUE;
}

int llwrite(int fd, unsigned char packet[], int index){
  printf("writing\n");
  return 0;
}
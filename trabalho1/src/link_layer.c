
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
Link_control link_control;
extern int fd;

int llopen(int type){


  fd = startConnection(type);
  link_control.N_s = 0;
  int res;
  //sender
  if(type == SENDER){
    //send SET
    printf("[STARTING CONNECTION]\n");
    printf("[SENDING SET]\n");
    res = sendControl();
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
    printf("[CLOSING CONNECTION]\n[INFO]\n  Sending DISC\n");
    write(fd, DISC, sizeof(DISC));
    //receive UA
    setAlarm(3);                 // activa alarme de 3s
    readMessage(fd, UA);
    cancelAlarm();
    printf("[INFO]\n  UA received\n");
  }
  //receiver
  else{
    //receive DISC
    setAlarm(3);                 // activa alarme de 3s
    readMessage(fd, DISC);
    cancelAlarm();
    printf("[INFO]\n  DISC received\n");
    //send UA
    printf("[INFO]\n  Sending UA\n");
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

int llwrite(int fd, unsigned char packet[], int packet_size){
  //compose frame
  //frame: [F, A, C, BCC1, [packet], BCC2, F]
  unsigned char frame[2 * packet_size + 6]; // 6 = F+A+C+BCC1+BCC2+F || 2*packet para assegurar que existe espaço suficiente para byte stuffing 
  //frame header
  frame[0] = FLAG;
  frame[1] = CONTROL;
  if(link_control.N_s == 0) frame[2] = C0;
  else frame[2] = C1;
  frame[3] = BCC(CONTROL, frame[2]);

  //process data
  unsigned int framePosition = 4;
  unsigned int packetPosition = 0;
  unsigned char current_packet_char;
  while(packetPosition < packet_size){
    current_packet_char = packet[packetPosition++];

    if(current_packet_char == FLAG || current_packet_char == ESC){
      frame[framePosition++] = ESC;
      frame[framePosition++] = current_packet_char ^ BYTE_STUFF;
    }
    else frame[framePosition++] = current_packet_char;

  } 
  //frame footer
  unsigned char bcc2 = 0x00;
  for (int i = 0; i < packet_size; i++){
    bcc2 ^= packet[i];
  }
  frame[framePosition++] = bcc2;
  frame[framePosition++] = FLAG;

  int res = write(fd, frame, framePosition);
  link_control.framesSent++;
  printf("[INFO]\n Sent frame number %d with size %d\n", link_control.framesSent, framePosition);
   
  return res;
  }

int sendControl(){
int res = write(fd, SET, sizeof(SET));
return res;
}


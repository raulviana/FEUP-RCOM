
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

unsigned char SET[5] = { FLAG, CONTROL, CONTROL_SET, BCC(CONTROL, CONTROL_SET), FLAG};
unsigned char UA[5] = {FLAG, CONTROL, CONTROL_UA, BCC(CONTROL, CONTROL_UA), FLAG};
unsigned char DISC[5] = {FLAG, CONTROL, CONTROL_DISC, BCC(CONTROL, CONTROL_DISC), FLAG};
unsigned char RR1[5] = {FLAG, CONTROL, C_R1, BCC(CONTROL, C_R1), FLAG};
unsigned char RR0[5] = {FLAG, CONTROL, C_R0, BCC(CONTROL, C_R0), FLAG};
unsigned char REJ0[5] = {FLAG, CONTROL, C_REJ1, BCC(CONTROL, C_REJ1), FLAG};
unsigned char REJ1[5] = {FLAG, CONTROL, C_REJ0, BCC(CONTROL, C_REJ0), FLAG};

struct termios newtio, oldtio;
enum state current;
Link_control link_control;
extern int fd;
extern int conta;


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
    setAlarm(TIMEOUT);                 // activa alarme de 3s
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


unsigned char COM_currentMachine(enum state* current, unsigned char buf){
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
				if((buf == CONTROL_SET) || (buf == CONTROL_UA) || (buf = CONTROL_DISC) || (buf == C0) || (buf == C1)){
          control_byte = buf;
          *current=READ_BCC;
        }
				else if(buf==FLAG){
					*current=READ_FLAG;
        }
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
          }
          else *current = FLAG;
  			break;

		}
  return control_byte;
}

void data_currentMachine(enum state* current, unsigned char buf) {
  unsigned char control_byte;

	switch(*current) {
		case START:
			if(buf == FLAG){
				*current = READ_FLAG;
			}
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
				if((buf == C0) || (buf == C1)){
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
      if(buf!=FLAG){
        *current = DATA;
        return;
      }
      else *current=START;
            
      break;

    case DATA:
      if(buf==FLAG){
        *current = STOP;
        return;
			}  
      break;

    case STOP:
      break;
		default:
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
  if(link_control.N_s == 0) link_control.N_s = 1;
  else link_control.N_s = 0;

  unsigned int continueFlag = TRUE;
  int res;
  unsigned int framePosition;;
  unsigned int tries = 0;
  unsigned char frame[2 * packet_size + 6]; // 6 = F+A+C+BCC1+BCC2+F || 2*packet para assegurar que existe espaço suficiente para byte stuffing 
   
  do{
    if(tries >= MAX_TRIES){
      printf("[ERROR]\n  Max tries reached, aborting\n");
      return -1;
    }
    framePosition = 4;
    //compose frame
    //frame: [F, A, C, BCC1, [packet], BCC2, F]
    //frame header
    frame[0] = FLAG;
    frame[1] = CONTROL;
    if(link_control.N_s == 0) frame[2] = C0;
    else frame[2] = C1;
    frame[3] = BCC(CONTROL, frame[2]);

    //process data
    
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
    unsigned char bcc2 = 0;
    for (int i = 0; i < packet_size; i++){//packing packet in frame
      bcc2 ^= packet[i];
    }
    //frame footer
    if(bcc2 == FLAG || bcc2 == ESC){
      frame[framePosition++] = ESC;
      frame[framePosition++] = bcc2 ^ BYTE_STUFF;
    }
    else frame[framePosition++] = bcc2;
    frame[framePosition++] = FLAG;
    printf("bbc2: %4X\n", frame[framePosition - 2]);
    for(int i = 0; i < framePosition; i++){
      printf("%4X  ", frame[i]);
    }
    setAlarm(TIMEOUT);
    res = write(fd, frame, framePosition);
    if(readResponse(fd) == -1){ //Received REJ, have to resend frame
      cancelAlarm();
      bzero(&frame, framePosition);
       printf("[ALERT]\n Re-sending frame number %d with size %d\n", link_control.framesSent, framePosition);
      tries++;
      continue;
    }
    else {
      cancelAlarm();
      continueFlag = FALSE;
    }
  }while( continueFlag);
  link_control.framesSent++;
  printf("[INFO]\n Sent frame number %d with size %d\n", link_control.framesSent, framePosition);
   
  return res;
}


int sendControl(){
  int res = write(fd, SET, sizeof(SET));
  return res;
}


int llread(int fd, unsigned char* packet){
  unsigned char frame[MAX_FRAME_SIZE];
  unsigned char control_field = 0x00;
  int done = FALSE;
  int frame_length = 0;
  int packet_length = 0;

  while(! done){
    frame_length = readFrame(fd, frame);
    if(frame_length == -1){
      return -1;
    } 
    
    //VErify data?????????????????????????????????????????????

  
    // destuff frame
    unsigned char final_frame[frame_length];
    int final_frame_length = destuffFrame(frame, frame_length, final_frame);

    control_field = frame[2];
    // //confirm data Integrity
    if(! confirmIntegrity(final_frame, final_frame_length)){
      printf("[ERROR]\n  Packet with error, asking re-emission\n");
      if(control_field == C0){
        write(fd, REJ1, sizeof(REJ1));
        link_control.RJsent++;
        printf("[INFO]\n REJ1 sent\n");
      }
      else if(control_field == C1){
        write(fd, REJ0, sizeof(REJ0));
        link_control.RJsent++;
        printf("[INFO]\n REJ0 sent\n");
      }
      return 0;
    }
    else{
      // //Get the packet from within the frame
    
      for (int i = 4; i < final_frame_length - 2; i++) {
        packet[packet_length] = final_frame[i];
        packet_length++;
      }
      // //send proper response()
      if(control_field == C1){
        write(fd, RR0, sizeof(RR0));
        link_control.RRsent++;
        printf("[INFO]\n  RR0 sent\n");
        done == TRUE;
        break;
      }
      else{
        write(fd, RR1, sizeof(RR1));
        link_control.RRsent++;
        printf("[INFO]\n  RR1 sent\n");
        done == TRUE;
        break;
      }
    }
  }
  return packet_length;
}


int readFrame(int fd, unsigned char* frame){
  enum state current = START;
  unsigned char byte_read = 0x00;
  int position = 0;
  unsigned char state_return;
  while (current != STOP){
    read(fd, &byte_read, 1);
    data_currentMachine(&current, byte_read);
    if(current == READ_FLAG && position != 0) position = 0;
    frame[position++] = byte_read;
  }
  link_control.framesReceived++;
  return position;
}


int  destuffFrame(unsigned char* frame, int frame_length, unsigned char* final_frame){

  final_frame[0] = frame[0];
  final_frame[1] = frame[1];
  final_frame[2] = frame[2];
  final_frame[3] = frame[3]; // FLAG, A, C, BCC1

  int j = 4, i = 4;

  while(i < frame_length - 1){
    if(frame[i] != ESC){
      final_frame[j++] = frame[i];
    }
    else{
      i++;
      if(frame[i] == (FLAG ^ BYTE_STUFF)) final_frame[j++] = FLAG;
      else if (frame[i] == (ESC ^ BYTE_STUFF)) final_frame[j++] = ESC;
    }
    i++;
  }

  final_frame[j++] = frame[i++];
  
  return j;
}

int readResponse(int fd){
  unsigned char byte_read, control_field;
  current = START;

  while(current != STOP){
    read(fd, &byte_read, 1);
    COM_currentMachine(&current, byte_read);
    if(current == READ_BCC){
      control_field = byte_read;
    }
  }
  if(control_field == C_R0 && link_control.N_s == 1){
    link_control.RRreceived++;
    printf("[INFO]\n  Received RR #%d\n", link_control.RRreceived);
    return 0;
  }
  else if(control_field == C_R1 && link_control.N_s == 0){
    link_control.RRreceived++;
    printf("[INFO]\n  Received RR #%d\n", link_control.RRreceived);
    return 0;
  }
  else{
    link_control.RJreceived++;
    printf("[INFO]\n  Received REJ #%d\n", link_control.RJreceived);
    return -1;
  }

}

int confirmIntegrity(unsigned char* final_frame, int final_frame_length){
  unsigned char adress_field = final_frame[1];
  unsigned char control_field = final_frame[2];
  unsigned char BCC1 = final_frame[3];

  if((BCC1 == BCC(adress_field, control_field)) && (control_field == C0 || control_field == C1)){
  //calculate expected bcc2 ( data packet is between 4 and size - 2 of frame)
    unsigned char expected_bcc2 = 0;
    for( int i = 4; i < final_frame_length - 2; i++){
      expected_bcc2 ^= final_frame[i]; 
    }
    unsigned char bcc2 = final_frame[final_frame_length - 2];
    printf("receivedbcc: %4X  expected bcc: %4X\n", bcc2, expected_bcc2);
    for (int i = 0; i < final_frame_length; i++){
      printf("%4X ", final_frame[i]);
    }
    if(bcc2 != expected_bcc2){
      printf("[ERROR]\n  Error in bcc2\n");
      return FALSE;
    }
  }
  else if ((control_field != C1) || (control_field != C0)){
    printf("[ERROR]\n  Error in control field received\n");
    return FALSE;
  }
  return TRUE;
}


#pragma once 

/******** Alterável *************/
#define FILENAME "pinguim.gif"   // "pinguim.gif"
#define SENDER_PORT "/dev/ttyS0"
#define RECEIVER_PORT "/dev/ttyS0"
#define BAUDRATE B1200
#define MAX_CHUNK_SIZE 112 //tem de caber o nome do ficheiro e o seu tamanho no pacote de controlo
                          //para  pinguim.gif min = 21;
#define T_PROP 5000000L // 500000 nanoseconds -> 500 microsends
#define TIME_CORRECTION 100 //to seconds
#define PROB_ERROR 2   // em percentagem
/********************************/

#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define MAX_FRAME_SIZE 2 * MAX_CHUNK_SIZE

#define CONTROL_SIZE 5
#define DATA_PACKET_SIZE 40


#define FALSE 0
#define TRUE 1

#define FLAG 0x7E
#define CONTROL_SET 0x02
#define CONTROL_DISC 0x0b
#define CONTROL_UA 0x07
#define CONTROL 0x03
#define START_CONTROL 0x02
#define END_CONTROL 0x03
#define BCC(X, Y) (X) ^ (Y)

#define C_R0	0x05
#define C_R1	0x85
#define C_REJ0	0x81
#define C_REJ1	0x01

#define C0 0x00
#define C1 0x40
#define BYTE_STUFF 0x20
#define ESC 0x7d

#define FILE_SIZE_FIELD 0x00
#define FILE_NAME_FIELD 0x01
#define DATA_FIELD 0x01


#define TIMEOUT 3
#define MAX_TRIES 5

#define RECEIVER 2
#define SENDER 1

extern struct termios oldtio,newtio;

typedef struct{
    char* send_fileName;
    char* receive_fileName;
    int open_fd;
    int close_fd;
    int fileSize;
} FileInfo;

enum phase{
  OPENING_CONNECTION,
  SENDING_DATA,
  CLOSING_CONNECTION
};

int fd;




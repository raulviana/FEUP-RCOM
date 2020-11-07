#pragma once

#include "constants.h"


extern int continueFlag;
extern int numTries;

#define SENDER 1
#define RECEIVER 2

enum state {
 	START,
 	READ_FLAG,
 	READ_CONTROL,
    READ_BCC,
 	BCC_OK,
	DATA,
	STOP
};

typedef struct {
	unsigned int N_s;
	unsigned int framesSent;
	unsigned int framesReceived;
	unsigned int RRsent;
	unsigned int RJsent;
	unsigned int RRreceived;
	unsigned int RJreceived;
}Link_control;

extern Link_control link_control;

int llopen(int type);

int llclose(int fd, int type);

int readMessage(int fd, unsigned char commandExpected[]);

int startConnection(int type);

int closeConnection(int fd);

int llwrite(int fd, unsigned char packet[], int index);

int sendControl();

unsigned char COM_currentMachine(enum state* current, unsigned char buf);
void data_currentMachine(enum state* current, unsigned char buf);

int llread(int fd, unsigned char* packet, int stage);

int readFrame(int fd, unsigned char* frame);

int  destuffFrame(unsigned char* frame, int frame_length, unsigned char* final_frame);

int readResponse(int fd);

int confirmIntegrity(unsigned char* final_frame, int final_frame_length, int stage);

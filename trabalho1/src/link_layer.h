#pragma once

enum state {
 	START,
 	READ_FLAG,
 	READ_CONTROL_A,
    READ_CONTROL,
 	BCC_OK,
	STOP
};


int llopen(int type);

int llclose(int fd);

void COM_currentMachine(enum state* current, unsigned char buf);

int readMessage(int fd, unsigned char commandExpected[]);

int startConnection(int type);

int check_bcc1(char control_message[], int size);
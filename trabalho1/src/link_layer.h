#pragma once

enum state {
 	START,
 	READ_FLAG,
 	READ_CONTROL,
    READ_BCC,
 	BCC_OK,
	STOP
};


int llopen(int type);

int llclose(int fd, int type);

void COM_currentMachine(enum state* current, unsigned char buf);

int readMessage(int fd, unsigned char commandExpected[]);

int startConnection(int type);

int closeConnection(int fd);
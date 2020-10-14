

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#include "constants.h"
#include "link_layer.h"
#include "alarm.h"

extern int conta;
int fd;

int sendControlPacket(int fd, int control_type, FileInfo FileInfo);

int main(int argc, char** argv)
{
    int c, res;
    struct termios oldtio,newtio;
    char buf[255];
    // if ( (argc < 2) ||
  	//      ((strcmp("/dev/ttyS10", argv[1])!=0) &&
  	//       (strcmp("/dev/ttyS11", argv[1])!=0) )) {
    //   printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    //   exit(1);
    // }

    (void) signal(SIGALRM, atende);  // instala  rotina que atende interrupcao

    printf("      -->SEnder<--\n");

    fd = llopen(SENDER);
    if(fd == -1){
      perror("[ERROR] Could not establish connection\n");
      exit(-1);
    }  //else -> Connection online
      
    printf("[CONNECTION ONLINE]\n");

    
    /*    +++++++DATA SEnding+++++++++++++++   */
    FileInfo fileInfo;
    struct stat meta_data;
    int file_fd;

    //open file
    file_fd = open(FILENAME, O_RDONLY);
    if(file_fd == -1){
      perror("[ERROR]  Error opening file, aborting\n");
      return -1;
    }

    if(fstat(file_fd, &meta_data) == -1){
      perror("[ERROR]  Error in fstat\n");
      return -1;
    }

    fileInfo.fileSize = meta_data.st_size;
    fileInfo.open_fd = file_fd;
    fileInfo.send_fileName = FILENAME;

    //construct and send opening control packet
    if(sendControlPacket(fd, START_CONTROL, fileInfo) == -1){
      perror("[ERROR]\n Error sending start control packet\n");
      exit(-1);
    }

    //send data packets

    
    //construct and send closing control packet
    // if(sendControlPacket(fd, END_CONTROL, fileInfo) == -1){
    //   perror("[ERROR]\n Error sending ending control packet\n");
    //   exit(-1);
    // }

    /*    +++++++++++++++++++++++++++++++++++   */


    llclose(fd, SENDER);
    printf("[CONNECTION CLOSED]\n");
    
    return 0;
}

int sendControlPacket(int fd, int control_type, FileInfo fileInfo){
  unsigned int index = 0;
  unsigned int file_size_length = sizeof(fileInfo.fileSize);
  //[C, T1, L1, ..., T2, L2, ...] = 5 (COntrol Size)
  unsigned char packet[CONTROL_SIZE + file_size_length + strlen(fileInfo.send_fileName)];

  packet[index++] = control_type;

  //insert file size T1, L1 and value
  packet[index++] = FILE_SIZE_FIELD;
  packet[index++] = file_size_length;
  unsigned char byteArray[file_size_length];
  printf("File size: %d\n", fileInfo.fileSize);
  //transformar int em array de chars
  for (int i = 0; i < file_size_length; i++){
		byteArray[i] = (fileInfo.fileSize >> 8*(file_size_length - 1 - i)); //masking
	}
  //colocar os chars no packet
  for (int i = 0; i < file_size_length; i++){
		packet[index++] = byteArray[i];
	}

  //insert file name
  packet[index++] = FILE_NAME_FIELD;
  packet[index++] = strlen(fileInfo.send_fileName);
  for (int i = 0; i < strlen(fileInfo.send_fileName); i++){
    packet[index++] = fileInfo.send_fileName[i];
  }
  int res = llwrite(fd, packet, index);
  if (res == -1){
    return -1;
  }
  return 0;
}



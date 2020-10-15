

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#include "constants.h"
#include "link_layer.h"


extern enum phase link_phase;

int check_bcc1(char control_message[], int size);
int readControlPacket();

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    // if ( (argc < 2) ||
  	//      ((strcmp("/dev/ttyS10", argv[1])!=0) &&
  	//       (strcmp("/dev/ttyS11", argv[1])!=0) )) {
    //   printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    //   exit(1);
    // }

    printf("      -->RECEIVER<--\n");

    fd = llopen(RECEIVER);
    if(fd == -1){
      perror("[ERROR] Could not establish connection\n");
      exit(-1);
    }  //else -> Connection online
      
    printf("[CONNECTION ONLINE]\n");

   
    /*    +++++++DATA Receiving+++++++++++++++   */
 
    //receive start control packet
    if(readControlPacket() == -1){
      perror("[ERROR]\n Error reading start control packet\n");
      exit(-1);
    }

    //receive data packets


    /*    +++++++++++++++++++++++++++++++++++   */


    llclose(fd, RECEIVER);
    printf("[CONNECTION CLOSED]\n");
   

    return 0;
}


int readControlPacket(){
  unsigned char packet[MAX_FRAME_SIZE];
  int res = llread(fd, packet);
  if(res == -1) return -1;
  
  int index = 1; //after flag
  int file_size = 0;
  if(! packet[index] == FILE_SIZE_FIELD) return -1;
  else{
      index++;
      int file_size_length = packet[index++];
      //transforming chars byte into an int again
      for(int i = 0; i < file_size_length; i++){
   	   file_size += packet[index++] << 8 * (file_size_length - 1 - i);
      }
  }
  if(! packet[index] == FILE_NAME_FIELD) return -1;
  else{
      index++;
      int name_length = packet[index++];
      char file_name[name_length + 1];
      for(int i = 0; i < name_length; i++){
   	   file_name[i] = packet[index++];
      }
      file_name[name_length] = '\0';
      printf("[INFO]\n Prepared to receive file: %s with size: %d\n", file_name, file_size);
  }
 return 0;
}



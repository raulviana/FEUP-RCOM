

#include "constants.h"
#include "link_layer.h"

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
#include <time.h>


extern enum phase link_phase;
FileInfo fileInfo;
struct timespec t0, t1;
int current_percentage_error;

extern int received_id = FALSE;

int readControlPacket();
int receiveFile(FileInfo fileInfo);
void processData(unsigned char* packet, FileInfo fileInfo);
void printStats(double time_taken);

int main(int argc, char** argv)
{
    srand(time(NULL));
    current_percentage_error = rand() % 101;
    int fd,c, res;
    struct termios oldtio,newtio;
    bzero(&fileInfo, sizeof(fileInfo));

    
    
    unsigned char packet[MAX_FRAME_SIZE + DATA_PACKET_SIZE];
    // if ( (argc < 2) ||
  	//      ((strcmp("/dev/ttyS10", argv[1])!=0) &&
  	//       (strcmp("/dev/ttyS11", argv[1])!=0) )) {
    //   printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    //   exit(1);
    // }

    printf("      -->RECEIVER<--\n");

	receive_id = TRUE;
    
    link_phase = OPENING_CONNECTION;
    fd = llopen(RECEIVER);
    if(fd == -1){
      perror("[ERROR] Could not establish connection\n");
      exit(-1);
    }  //else -> Connection online
      
    printf("[CONNECTION ONLINE]\n");

   
    /*    +++++++DATA Receiving+++++++++++++++   */
   
    //receive start control packet
    link_phase = OPENING_CONNECTION;
    link_control.N_s = 0;
    
    if(readControlPacket() == -1){
      perror("[ERROR]\n Error reading start control packet\n");
      exit(1);
    }
    printf("[INFO]\n  Ready to receive data\n");
    //receive data packets
    link_phase = SENDING_DATA;
    link_control.RJreceived =0;
    link_control.RJsent = 0;
    link_control.RRreceived =0;
    link_control.RRsent = 0;
    link_control.framesReceived =0;
    
    clock_gettime(0, &t0);

    if(receiveFile(fileInfo) == -1){
      printf("[ERROR]\n  Error in llread\n");
      exit(2);
    }
    
    clock_gettime(0, &t1);
    double time_taken = ((double) t1.tv_sec - t0.tv_sec) + ((double)(t1.tv_nsec - t0.tv_nsec) /1000000000L);
    /*    +++++++++++++++++++++++++++++++++++   */
   
    llclose(fd, RECEIVER);
    printf("[CONNECTION CLOSED]\n");
    
    printStats(time_taken);

    return 0;
}


int readControlPacket(){
  unsigned char packet[MAX_FRAME_SIZE];
  int res = llread(fd, packet, OPENING_CONNECTION);
  if(res == -1){
    return -1;
  } 
  
  int index = 1; //after flag
  int file_size = 0;

  if( packet[index] != FILE_SIZE_FIELD){
    return -1;
  } 
  else{
      index++;
      int file_size_length = packet[index++];
      //transforming chars byte into an int again
      for(int i = 0; i < file_size_length; i++){
   	   file_size += packet[index++] << 8 * (file_size_length - 1 - i);
      }
  }
  if( packet[index] != FILE_NAME_FIELD) return -1;
  else{
      index++;
      int name_length = packet[index++];
      char file_name[name_length + 1];
      for(int i = 0; i < name_length; i++){
   	   file_name[i] = packet[index++];
      }
      file_name[name_length] = '\0';
      //testing
      file_name[0] = '1';
      //
      fileInfo.receive_fileName = file_name;
      remove(fileInfo.receive_fileName);
      fileInfo.close_fd = open(fileInfo.receive_fileName, O_RDWR | O_CREAT , 777);
      printf("[INFO]\n Prepared to receive file: %s with size: %d\n", file_name, file_size);
  }
 return 0;
}

int receiveFile(FileInfo fileInfo){
  unsigned char max_buf[MAX_CHUNK_SIZE + DATA_PACKET_SIZE];

  unsigned int bytes_read = 0;
  unsigned int received = 0;
  int aux = 0;
  while(! received){
    if((aux = llread(fd, max_buf, SENDING_DATA)) != 0){

      printf("[INFO]\n  Received packet #%d\n", link_control.framesReceived);
      
      bytes_read += aux;
      if(max_buf[0] == DATA_FIELD) {
        processData(max_buf, fileInfo);
      }
      else if(max_buf[0] == END_CONTROL){
        received = 1;
      }
    }
  }
  close(fileInfo.close_fd);
  return 0;
}


void processData(unsigned char* packet, FileInfo fileInfo){

	int dataSize = 256 * packet[2] + packet[3];
  
	int res = write(fileInfo.close_fd, &packet[4], dataSize);
}

void printStats(double time_taken){
  printf("\n     ***Statistics***\n");
  printf("Total transfer time: %f seconds\n",time_taken);
  printf("Number of frames received: %d\n", link_control.framesReceived);
  printf("Number of RR frames sent: %d\n", link_control.RRsent);
  printf("Number of REJ frames sent: %d\n", link_control.RJsent);
  printf("      **************\n");
  
  
}


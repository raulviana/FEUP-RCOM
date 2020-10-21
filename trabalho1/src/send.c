

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

#include "constants.h"
#include "link_layer.h"
#include "alarm.h"

extern enum phase link_phase;
int fd;
clock_t tic, toc;

int sendControlPacket(int fd, int control_type, FileInfo FileInfo);
int sendFile(FileInfo fileInfo);
void printStats();

int main(int argc, char** argv)
{
    int c, res;
    struct termios oldtio,newtio;
    // if ( (argc < 2) ||
  	//      ((strcmp("/dev/ttyS10", argv[1])!=0) &&
  	//       (strcmp("/dev/ttyS11", argv[1])!=0) )) {
    //   printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    //   exit(1);
    // }

    (void) signal(SIGALRM, atende);  // instala  rotina que atende interrupcao

    printf("      -->SEnder<--\n");

    link_phase = OPENING_CONNECTION;
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
    
    //start counting time
    tic = clock();
    //construct and send opening control packet
    link_phase = SENDING_DATA;
    link_control.N_s = 1;
    if(sendControlPacket(fd, START_CONTROL, fileInfo) == -1){
      perror("[ERROR]\n Error sending start control packet\n");
      exit(1);
    }

    //send file
    link_phase = SENDING_DATA;
    link_control.N_s = 1;
    link_control.framesReceived = 0;
    link_control.framesSent = 0;
    link_control.RJreceived =0;
    link_control.RJsent = 0;
    link_control.RRreceived =0;
    link_control.RRsent = 0;
    if(sendFile(fileInfo) == -1){
      printf("[ERROR]\n  Error in llwrite\n");
      exit(2);
    }
    //construct and send closing control packet
    if(sendControlPacket(fd, END_CONTROL, fileInfo) == -1){
      perror("[ERROR]\n Error sending ending control packet\n");
      exit(-1);
    }
    //stop counting time
    toc = clock();
    /*    +++++++++++++++++++++++++++++++++++   */


    llclose(fd, SENDER);
    printf("[CONNECTION CLOSED]\n");

    printStats();
    
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

int sendFile(FileInfo fileInfo){
  unsigned char buffer[MAX_CHUNK_SIZE];
  unsigned int chunks_sent = 0;
  unsigned int chunks_to_send = fileInfo.fileSize / MAX_CHUNK_SIZE + (fileInfo.fileSize % MAX_CHUNK_SIZE != 0); //se na divisão pelo tamanho máximo sobrarem bytes é necessário adicionar mais um chunk com menos bytes do que o tamanho máximo 

  unsigned int byte_read = 0;
  unsigned int bytes_written = 0;
  unsigned int total = 0;

  printf("[INFO]\n  Sending file %s with %d bytes in %d splited parts\n", FILENAME, fileInfo.fileSize, chunks_to_send);
  
  while(chunks_sent < chunks_to_send){
    byte_read = read(fileInfo.open_fd, &buffer, MAX_CHUNK_SIZE);
    unsigned char packet[DATA_PACKET_SIZE + byte_read];

    //construct packet
    // [C, N, L2, L1, P1, ..., Pk]
    packet[0] = DATA_FIELD;
    packet[1] = chunks_sent % 255;
    packet[2] = byte_read / 256;
    packet[3] = byte_read % 256; //última posição é 256 * L2 + L1
    memcpy(&packet[4], &buffer, byte_read); //coloca o conteudo do buffer nas posições seguintes do packet

    bytes_written = llwrite(fd, packet,byte_read + DATA_PACKET_SIZE);
    if(bytes_written == -1)return -1;
    total += bytes_written;
    chunks_sent++; 
  }
  printf("[INFO]\n  Sent %d data bytes in %d chunks\n", total - DATA_PACKET_SIZE * chunks_sent,chunks_sent);
}

void printStats(){
  printf("\n     ***Statistics***\n");
  printf("Total transfer time: %f seconds\n", (double)(toc - tic) * TIME_CORRECTION / CLOCKS_PER_SEC);
  printf("Number os frames sent: %d\n", link_control.framesSent);
  printf("Number of RR frames received: %d\n", link_control.RRreceived);
  printf("Number of REJ frames received: %d\n", link_control.RJreceived);
  printf("     **************\n");
}


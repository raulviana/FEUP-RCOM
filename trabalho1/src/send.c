

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

int check_bcc1(char control_message[], int size);

int main(int argc, char** argv)
{
    int fd,c, res;
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

    //send data packets

    //construct and send closing control packet



    /*    +++++++++++++++++++++++++++++++++++   */




    llclose(fd, SENDER);
    printf("[CONNECTION CLOSED]\n");
    
    return 0;
}

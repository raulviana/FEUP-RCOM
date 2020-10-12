

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

extern int conta;

int check_bcc1(char control_message[], int size);

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
  //TO-DO
}

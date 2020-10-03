/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG 0x7E
#define CONTROL_SET 0x02
#define CONTROL_DISC 0x0b
#define CONTROL_UA 0x07
#define FIELD_A_SC 0x03
#define FIELD_A_RC 0x01


volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd, c, res;
    struct termios oldtio,newtio;
    int i, sum = 0, speed = 0;
    char buf[255];
    
    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS10", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS11", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttySX\n");
      exit(1);
    }

  printf("-->SENDER<--\n");

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */



  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) pr�ximo(s) caracter(es)
  */


    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
    printf("New termios structure is set\n");

    char SET[5];
    SET[0] = FLAG;
    SET[1] = FIELD_A_SC;
    SET[2] = CONTROL_SET;
    SET[3] = 0x00;
    SET[4] = FLAG;

    /*******************
     * Send Message
     * ****************/
    printf("[STARTING CONNECTION]  Sending SET");

    res = write(fd, SET, sizeof(SET));
    printf(" %d bytes writen\n", res);
    
    exit(1);
    char in_message[255];
    int position = 0;

    printf("Read send back message:\n");

    while (STOP==FALSE) {       /* loop for input */
      res = read(fd,buf,1);   /* returns after 1 chars have been input */
      buf[res] = 0;               /* so we can printf... */
      printf("%s:%d\n", buf, res);
      in_message[position++] = buf[0];

      if(buf[0] == '\0') STOP=TRUE;
    }

    printf("Mesage received: %s with %d bytes\n", in_message, res);
  


   //Voltar a colocar a estrutura termios no estado inicial
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }




    close(fd);
    return 0;
}

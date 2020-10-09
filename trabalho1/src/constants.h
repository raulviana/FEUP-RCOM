

#define FILENAME "pinguim.gif"
#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1


#define FLAG 0x7E
#define CONTROL_SET 0x02
#define CONTROL_DISC 0x0b
#define CONTROL_UA 0x07
#define CONTROL_A 0x03
#define BCC(X, Y) (X) ^ (Y)


#define TIMEOUT 3
#define MAX_TRIES 3


#define SENDER_PORT "/dev/ttyS10"
#define RECEIVER_PORT "/dev/ttyS11"
#define RECEIVER 2
#define SENDER 1

extern struct termios oldtio,newtio;


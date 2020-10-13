
#define FILENAME "test.txt"   // "pinguim.gif"
#define SENDER_PORT "/dev/ttyS0"
#define RECEIVER_PORT "/dev/ttyS0"

#define BAUDRATE B38400
#define MAX_FRAME_SIZE 512
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define FALSE 0
#define TRUE 1

#define FLAG 0x7E
#define CONTROL_SET 0x02
#define CONTROL_DISC 0x0b
#define CONTROL_UA 0x07
#define CONTROL 0x03
#define START_CONTROL 0x02
#define END_CONTROL 0x03
#define BCC(X, Y) (X) ^ (Y)

#define CONTROL_SIZE 5
#define FILE_SIZE_FIELD 0x00
#define FILE_NAME_FIELD 0x01


#define TIMEOUT 3
#define MAX_TRIES 3


#define RECEIVER 2
#define SENDER 1

extern struct termios oldtio,newtio;

typedef struct{
    char* send_fileName;
    char* receive_fleName;
    int open_fd;
    int close_fd;
    int fileSize;
} FileInfo;

int fd;

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <strings.h>


int main(){
size_t sentFileSize = 3400;
int file_size_length = sizeof(sentFileSize);
unsigned char byteArray[file_size_length];

for (int i = 0; i < file_size_length; i++){
		byteArray[i] = (sentFileSize >> 8*(file_size_length - 1 - i)) & 0xFF;
	}
for(int i = 0; i < file_size_length; i++){
     printf("%4X ", byteArray[i]);
}
printf("\n");


int index = 0;
int file= 0;
for (int i = 0; i < file_size_length; i++){
			file += byteArray[index] << 8 * (file_size_length - 1 - i);
			index++;
}

printf("SIze: %d\n", file);
return 0;
}
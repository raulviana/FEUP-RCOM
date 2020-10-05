
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#include <unistd.h>
#include <unistd.h>




int main(){

    struct stat meta;

    int fd_file = open("pinguim.gif", O_RDONLY);

    fstat(fd_file, &meta);
    
    char buf[meta.st_size];

    read(fd_file, &buf, sizeof(buf));

    printf("FileSIze: %ld\n", meta.st_size);

    printf("Buf size: %ld\n", sizeof(buf));

    for(int i = 0; i < sizeof(buf); i++){
        printf("%d", buf[i]);
    }
   


    
    close(fd_file);

    return 0;
}
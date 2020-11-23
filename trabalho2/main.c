/*      (C)2000 FEUP  */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <string.h>

#include "url_handler.h"

#define SERVER_PORT 6000
#define SERVER_ADDR "192.168.28.96"
#define MAX_STRING_LENGTH 256

//gets ip address according to the host's name
int getip(char host[])
{
	struct hostent *h;

	if ((h = gethostbyname(host)) == NULL)
	{
		herror("gethostbyname");
		exit(1);
	}

    printf("\nHost name  : %s\n", h->h_name);
    printf("IP Address : %s\n",inet_ntoa(*((struct in_addr *)h->h_addr)));

	return 0;
}



int main(int argc, char** argv){

	int	sockfd;
	struct	sockaddr_in server_addr;
	char buf[] = "Mensagem de teste na travessia da pilha TCP/IP\n";  
	int	bytes;

    url url;

    create_url_struct(&url);

    parseURL(argv[1], &url);

    char *path = url.url_path;

	parseFilename(path, &url);

    printf(" - Username: %s\n", url.user);
	printf(" - Password: %s\n", url.password);
	printf(" - Host: %s\n", url.host);
	printf(" - Path: %s\n", url.url_path);
    printf(" - Filename: %s\n", url.filename);

    getip(url.host);
	
	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(SERVER_PORT);		/*server TCP port must be network byte ordered */
    
	/*open an TCP socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    		perror("socket()");
        	exit(0);
    	}
	/*connect to the server*/
    	if(connect(sockfd, 
	           (struct sockaddr *)&server_addr, 
		   sizeof(server_addr)) < 0){
        	perror("connect()");
		exit(0);
	}
    	/*send a string to the server*/
	bytes = write(sockfd, buf, strlen(buf));
	printf("Bytes escritos %d\n", bytes);

	close(sockfd);
	exit(0);
}





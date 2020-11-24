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

/* gets ip address according to the host's name */
int getip(char host[], url* url)
{
	struct hostent *h;

	if ((h = gethostbyname(host)) == NULL)
	{
		perror("gethostbyname");
		exit(1);
	}

    strcpy(url->ip_address, inet_ntoa(*((struct in_addr *)h->h_addr_list[0])));

	return 0;
}

int ftp_send_command(int socket_id, const char* command, int command_size);
int open_socket(const char* ip_address, const int port);


int main(int argc, char** argv){

	if(argc < 2){
		printf("Error: You must input an url.\n Usage: ./download ftp://ftp.up.pt/pub/..\n");
		exit(2);
	}

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
    getip(url.host, &url);
    printf(" - IP: %s\n", url.ip_address);
	
/***************/

	/* start connection */
	int socket_id;
	char buffer[MAX_STRING_LENGTH];
	if((socket_id = open_socket(url.ip_address, FTP_PORT)) < 0){
		perror("Error in open_socket()\n");
		exit(2);
	}

	printf("FTP connection established\n");

	/* Log In in ftp server */
	char command[MAX_COMMAND_LENGTH];
	//user
	sprintf(command, "user %s", url.user);
	if(ftp_send_command(socket_id, command, strlen(command))){
		perror("Error sending command USER\n");
		exit(3);
	}
	//pass
	bzero(command, MAX_COMMAND_LENGTH);
	sprintf(command, "pass %s", url.password);
	if(ftp_send_command(socket_id, command, strlen(command))){
		perror("Error sending command PASSWORD\n");
		exit(4);
	}



}



/* Auxiliary functions */

int open_socket(const char* ip_address, const int port){
	 int socket_fd;
	struct sockaddr_in server_addr;
	// server address handling
	bzero((char*) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip_address); /*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port); /*server TCP port must be network byte ordered */
	// open a TCP socket
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket()\n");
		return -1;
	}
	// connect to the server
	if (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		perror("connect()\n");
		return -1;
	}
	return socket_fd;
}

int ftp_send_command(int socket_id, const char* command, int command_size){
	int res = write(socket_id, command, command_size);
	if( res <= 0){
		perror("Error in ftp_send_command()\n");
		return TRUE;
	}
	printf("Written %d bytes: %s\n", res, command);
	return FALSE;
}

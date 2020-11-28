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

int ftp_send_command(int socket_id, const char* command, int command_size, char* response);
int open_socket(const char* ip_address, const int port);
int read_socket(int socket_id, char* response);
int ftp_passive_mode(int socket_id, char* command, size_t command_size, char* response);

int new_socket_id;

int main(int argc, char** argv){

	if(argc < 2){
		printf("Error: You must input an url.\n Usage: ./download ftp://ftp.up.pt/pub/..\n");
		exit(2);
	}

	int	sockfd;
	struct	sockaddr_in server_addr;
	char buf[] = "Mensagem de teste na travessia da pilha TCP/IP\n";  
	int	bytes;
	
	char command[MAX_COMMAND_LENGTH];
	char response[MAX_STRING_LENGTH];

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

    printf("\n");
	read_socket(socket_id, response);

	printf("FTP connection established\n");

	/* Log In in ftp server */
	
	//user
	sprintf(command, "USER %s\r\n", url.user);
	if(ftp_send_command(socket_id, command, strlen(command), response)){
		perror("Error sending command USER\n");
		exit(3);
	}
	//pass
	bzero(command, MAX_COMMAND_LENGTH);
	bzero(response, MAX_STRING_LENGTH);
	sprintf(command, "PASS %s\r\n", url.password);
	
	if(ftp_send_command(socket_id, command, strlen(command), response)){
		perror("Error sending command PASSWORD\n");
		exit(4);
	}

	//enter passive mode
	bzero(command, MAX_COMMAND_LENGTH);
	bzero(response, MAX_STRING_LENGTH);
	sprintf(command, "PASV\r\n", url.password);
	if (ftp_passive_mode(socket_id, command, strlen(command), response)) {
		perror("Error in ftp_passive_mode()\n");
		exit(5);
    }
    printf("Entered passive mode successfully\n");

	printf("new_socket: %d\n", new_socket_id);

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

int ftp_send_command(int socket_id, const char* command, int command_size, char* response){
	int res = write(socket_id, command, command_size);
	if( res <= 0){
		perror("Error in ftp_send_command()\n");
		return TRUE;
	}
	printf("Written %d bytes: %s\n", res, command);
	
	printf("Server response: \n");
	if(read_socket(socket_id, response)){
		perror("Error reading from socket\n");
		return TRUE;
	};
	printf("\n");
	return FALSE;
}

int read_socket(int socket_id, char* response){
	FILE* fp = fdopen(socket_id, "r");
	do{
		memset(response, 0, MAX_STRING_LENGTH);
		response = fgets(response, MAX_STRING_LENGTH, fp);
		printf("%s", response);
	}  while (!('1' <= response[0] && response[0] <= '5') || response[3] != ' ');
	// response[3] == ' ' means received a last status line
	//response [0] <= 5 means received numerated status line
	printf("\n\n");
	return 0;
}

int ftp_passive_mode(int socket_id, char* command, size_t command_size, char* response){
	if(ftp_send_command(socket_id, command, command_size, response)){
		perror("Error in ftp_send_command()");
		exit(4);
	}
	
	char ip_address[MAX_STRING_LENGTH];
	int port_num;
	int ip1, ip2, ip3, ip4;
	int port1, port2;

	//copy values from response to corresponding variables
	if (sscanf(response, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &ip1, &ip2, &ip3, &ip4, &port1, &port2) < 0) {
		perror("sscanf()\n");
		return 1;
	}

	// Creating server ip address
	sprintf(ip_address, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);

	// Calculating tcp port number
	port_num = port1 * 256 + port2;

	printf("IP: %s\n", ip_address);
	printf("PORT: %d\n", port_num);

	if ((new_socket_id = open_socket(ip_address, port_num)) < 0) {
		perror("open_socket()\n");
		return 1;
	}

	return 0;
}

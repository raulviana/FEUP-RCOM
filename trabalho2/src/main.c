

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
#include <sys/stat.h>
#include <fcntl.h>

#include "handlers.h"


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
    printf("Entered passive mode successfully\n\n");

	//reconstruct file path
	char filepath[MAX_STRING_LENGTH];
	bzero(command, MAX_COMMAND_LENGTH);
	bzero(response, MAX_STRING_LENGTH);
    sprintf(filepath, "%s", url.url_path);

	if (ftp_retrieve_file(socket_id, filepath, command, response)) {
        perror("ftp_retr_file()");
        exit(6);
    }

	//download file
	char filename[MAX_STRING_LENGTH];
	strcpy(filename, url.filename);
	if (ftp_download_file(new_socket_id, filename)) {
        perror("ftp_download_file()\n");
        exit(7);
    }
    printf("Downloaded file %s successfully\n", url.filename);

	close(socket_id);
}


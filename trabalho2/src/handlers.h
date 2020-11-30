#pragma once

#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>


#define MAX_STRING_LENGTH 255
#define MAX_COMMAND_LENGTH 512

#define TRUE 1
#define FALSE 0

#define FTP_PORT 21

typedef struct url{
	char user[MAX_STRING_LENGTH]; 
	char password[MAX_STRING_LENGTH]; 
	char host[MAX_STRING_LENGTH]; 
	char url_path[MAX_STRING_LENGTH]; 
    char filename[MAX_STRING_LENGTH]; 
    char ip_address[MAX_STRING_LENGTH]; 
} url;
extern int new_socket_id;

//url handling
void create_url_struct(url* url);
void parseURL(char *argument, url *url);
void parseFilename(char *path, url *url);

//ftp handling
int ftp_send_command(int socket_id, const char* command, int command_size, char* response);
int ftp_retr(int socket_id, const char* command, int command_size);
int ftp_passive_mode(int socket_id, char* command, size_t command_size, char* response);
int ftp_retrieve_file(int socket_id, char* filepath, char* command, char* response);
int ftp_download_file(int new_socket_id, char* filename);

//socket handling
int open_socket(const char* ip_address, const int port);
int read_socket(int socket_id, char* response);

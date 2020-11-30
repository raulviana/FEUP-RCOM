#include "handlers.h"

void create_url_struct(url* url) {
    memset(url->user, 0, MAX_STRING_LENGTH);
	memset(url->password, 0, MAX_STRING_LENGTH);
	memset(url->host, 0, MAX_STRING_LENGTH);
	memset(url->url_path, 0, MAX_STRING_LENGTH);
    memset(url->filename, 0, MAX_STRING_LENGTH);
    memset(url->ip_address, 0, MAX_STRING_LENGTH);
}


void parseURL(char *argument, url *url){

    // ftp://[<user>:<password>@]<host>/<url-path>

	char *user, *password, *host, *url_path;
    int length = strlen(argument);
    bool userAndPass = false;

    if(argument[0] != 'f' || argument[1] != 't' || argument[2] != 'p' ||
        argument[3] != ':' || argument[4] != '/' || argument[5] != '/'){
            printf("Error parsing ftp://\n");
            exit(1);
        }

    //Checks if the url has the user and password part
    for(int i = 0; i < length; i++){
        if(argument[i] == '@'){
            userAndPass = true;
        }
    }

    if(userAndPass){
        //Get user

        const char user_deli[] = ":"; 
        user = strtok(&argument[6], user_deli); 
        strcpy(url->user, user);

        //Get password

        const char pass_deli[] = "@"; 
        int pass_start_index = strlen(user) + 7;
        password = strtok(&argument[pass_start_index], pass_deli); 
        strcpy(url->password, password);

        //Get host

        const char host_deli[] = "/"; 
        int host_start_index = pass_start_index + strlen(password) + 1;
        host = strtok(&argument[host_start_index], host_deli); 
        strcpy(url->host, host);

        //Get url-path

        const char url_deli[] = "\n"; 
        int url_start_index = host_start_index + strlen(host) + 1;
        url_path = strtok(&argument[url_start_index], url_deli); 
        strcpy(url->url_path, url_path);
}
    else{
        //Get host

        const char host_deli[] = "/"; 
        host = strtok(&argument[6], host_deli); 
        strcpy(url->host, host);

        //Get url-path

        const char url_deli[] = "\n"; 
        int url_start_index = strlen(host) + 7;
        url_path = strtok(&argument[url_start_index], url_deli); 
        strcpy(url->url_path, url_path);
        
    }
}

void parseFilename(char *path, url *url){
    char filename[MAX_STRING_LENGTH];
    char path_to_remove[MAX_STRING_LENGTH];

    strcpy(filename, path);

    while (strchr(filename, '/')) {

        const char path_deli[] = "/"; 

        strcpy(path_to_remove, strtok(&filename[0], path_deli)); 
		
        strcpy(filename, filename + strlen(path_to_remove) + 1);
	}
    strcpy(url->filename, filename);
}


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

int ftp_retrieve_file(int socket_id, char* filepath, char* command, char* response){

	sprintf(command, "RETR %s\r\n", filepath);
	if (ftp_retr(socket_id, command, strlen(command))){
		perror("ftp_command()\n");
		return 1;
	}

	return 0;
}

int ftp_retr(int socket_id, const char* command, int command_size){
	int res = write(socket_id, command, command_size);
	if( res <= 0){
		perror("Error in ftp_retr()\n");
		return TRUE;
	}
	printf("Written %d bytes: %s\n", res, command);
	
	printf("\n");
	return FALSE;
}

int ftp_download_file(int new_socket_id, char* filename){
	char buffer[MAX_STRING_LENGTH];
	int file_fd;
	int res;

	if((file_fd = open(filename, O_WRONLY | O_CREAT, 0666)) < 0) {
		perror("open()\n");
		return 1;
	}
	while ((res = read(new_socket_id, buffer, sizeof(buffer)))) {

		if (res < 0) {
			perror("read()\n");
			return 1;
		}

		if (write(file_fd, buffer, res) < 0) {
			perror("write()\n");
			return 1;
		}

	}

	close(file_fd);
	close(new_socket_id);
	return 0;
}

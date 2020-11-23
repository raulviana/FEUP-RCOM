#include "url_handler.h"

void create_url_struct(url* url) {
    memset(url->user, 0, MAX_STRING_LENGTH);
	memset(url->password, 0, MAX_STRING_LENGTH);
	memset(url->host, 0, MAX_STRING_LENGTH);
	memset(url->url_path, 0, MAX_STRING_LENGTH);
}

void parseURL(char *argument, url *url){
    char str[] = "ftp://";
	char *user, *password, *host, *url_path;
	int length = strlen(argument);

    const char user_deli[] = ":"; 

    user = strtok(&argument[6], user_deli); 

    strcpy(url->user, user);

    const char pass_deli[] = "@"; 

    int pass_start_index = strlen(user) + 7;

    password = strtok(&argument[pass_start_index], pass_deli); 

    strcpy(url->password, password);

    const char host_deli[] = "/"; 

    int host_start_index = pass_start_index + strlen(password) + 1;

    host = strtok(&argument[host_start_index], host_deli); 

    strcpy(url->host, host);

    const char url_deli[] = "\n"; 

    int url_start_index = host_start_index + strlen(host) + 1;

    url_path = strtok(&argument[url_start_index], url_deli); 

    strcpy(url->url_path, url_path);
}
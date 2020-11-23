#include "url_handler.h"

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
    char *filename;
    char * path_to_remove;

    strcpy(filename, path);

    while (strchr(filename, '/')) {

        const char path_deli[] = "/"; 

        path_to_remove = strtok(&filename[0], path_deli); 
		
        strcpy(filename, filename + strlen(path_to_remove) + 1);
	}

    strcpy(url->filename, filename);
}

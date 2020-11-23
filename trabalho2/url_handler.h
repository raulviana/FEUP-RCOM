#pragma once

#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define MAX_STRING_LENGTH 256

typedef struct url{
	char user[MAX_STRING_LENGTH]; 
	char password[MAX_STRING_LENGTH]; 
	char host[MAX_STRING_LENGTH]; 
	char url_path[MAX_STRING_LENGTH]; 
} url;

void create_url_struct(url* url);

void parseURL(char *argument, url *url);
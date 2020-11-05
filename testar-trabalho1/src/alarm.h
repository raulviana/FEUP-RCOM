#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <strings.h>

#include "constants.h"
#include "link_layer.h"


extern unsigned char REJ0[5];
extern unsigned char REJ1[5];

void setAlarm(int seconds);

void cancelAlarm();

void atende();

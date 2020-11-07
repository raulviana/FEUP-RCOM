#pragma once

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

extern int continueFlag;
extern int numTries;

void setAlarm(int seconds);

void cancelAlarm();

void atende();
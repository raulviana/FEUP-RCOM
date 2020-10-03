#!/bin/bash

rm -rf ./receive

rm -rf ./send

gcc send.c -o send

gcc receive.c -o receive

gnome-terminal -x sh -c "./receive /dev/ttyS11; bash"

sleep 2

gnome-terminal -x sh -c "./send /dev/ttyS10; bash"



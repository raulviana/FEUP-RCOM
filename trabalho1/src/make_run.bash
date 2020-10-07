#!/bin/bash

make

gnome-terminal -x sh -c "./receive /dev/ttyS11; bash"

sleep 2

gnome-terminal -x sh -c "./send /dev/ttyS10; bash"



#!/bin/bash


gnome-terminal -x sh -c -- "sudo socat -d  -d  PTY,link=/dev/ttyS10,mode=777   PTY,link=/dev/ttyS11,mode=777; bash"







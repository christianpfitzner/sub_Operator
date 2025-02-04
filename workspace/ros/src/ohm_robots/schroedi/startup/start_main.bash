#!/bin/bash
SHELL=/bin/bash
/usr/bin/tmux new -d -s drives
/usr/bin/tmux new -d -s arm

/usr/bin/tmux send-keys -t drives "source /home/user/start_drives.bash" ENTER
#/usr/bin/tmux send-keys -t arm "roslaunch schroedi arm.launch" ENTER
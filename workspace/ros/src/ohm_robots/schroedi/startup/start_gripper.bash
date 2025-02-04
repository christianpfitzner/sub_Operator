#!/bin/bash
SHELL=/bin/bash
while ! ping -c 1 192.168.1.80; do
            echo "Waiting for network interface..."
                sleep 1
        done
#/usr/bin/tmux send-keys -t gripper "source /opt/ros/melodic/setup.bash" ENTER	
/usr/bin/tmux send-keys -t gripper "sleep 10" ENTER
/usr/bin/tmux send-keys -t gripper "roslaunch schroedi gripper_complete.launch" ENTER	

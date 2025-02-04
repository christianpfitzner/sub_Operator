#!/bin/bash
SHELL=/bin/bash
while ! ping -c 1 192.168.1.80; do
            echo "Waiting for network interface..."
                sleep 1
        done
#/usr/bin/tmux send-keys -t sensor-head "source /opt/ros/melodic/setup.bash" ENTER	
#/usr/bin/tmux send-keys -t sensor-head "sleep 10" ENTER
/usr/bin/tmux send-keys -t sensor-head "sleep 10" ENTER
/usr/bin/tmux send-keys -t sensor-head "roslaunch schroedi sensor-head.launch" ENTER	

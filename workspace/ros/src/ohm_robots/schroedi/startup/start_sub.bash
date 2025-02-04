#!/bin/bash
SHELL=/bin/bash
/usr/bin/tmux new -d -s roscore
/usr/bin/tmux new -d -s sensor-head
#sleep(2)
/usr/bin/tmux new -d -s gripper
#/usr/bin/tmux send-keys -t roscore "source /opt/ros/melodic/setup.bash" ENTER
#/usr/bin/tmux send-keys -t roscore "sleep 5" ENTER
#/usr/bin/tmux send-keys -t roscore "roscore" ENTER
/usr/bin/tmux send-keys -t roscore "source /home/user/start_roscore.bash" ENTER


#/usr/bin/tmux new -d -s sensor-head
/usr/bin/tmux send-keys -t sensor-head "source /home/user/start_sensor_head.bash" ENTER

#/usr/bin/tmux new -d -s gripper
#/usr/bin/tmux send-keys -t gripper "sleep 10" ENTER
#/usr/bin/tmux send-keys -t gripper "roslaunch schroedi gripper_complete.launch" ENTER
/usr/bin/tmux send-keys -t gripper "source /home/user/start_gripper.bash" ENTER

#ifndef MOVE_H
#define MOVE_H

#include <vector>
#include <sensor_msgs/JointState.h>
#include <sensor_msgs/Joy.h>
#include "Aktuelle_Position/Aktuelle_Position.h"
#include <ros/ros.h>
#include <ohm_move/RobotarmCtrl.h>

//Part 2
#include "ohm_teleop_msgs/ArmCommand.h"
#include <stddef.h>
#include <stdlib.h>
#include "ohm_actors_msgs/ActionGripperAction.h"
#include <actionlib/client/simple_action_client.h>

class Move
{

public:
    Move();
    ~Move();
    void set_initialize_joystick(sensor_msgs::Joy init_joystick);
    void set_joystick_input(sensor_msgs::Joy joy_input);
    void set_new_goal_position();
    void set_zero_position(sensor_msgs::Joy init_zero_position);
    void set_start_position(sensor_msgs::Joy init_start_position);
    void set_show_down_position(sensor_msgs::Joy init_show_down_position);
    void set_show_up_position(sensor_msgs::Joy init_show_up_position);
    void set_show_side_position(sensor_msgs::Joy init_show_side_position);
    void set_joint_angles(sensor_msgs::JointState js_update);
    void set_check_space_constraints();
    void set_angle_to_zero_positon_false();
    void set_angle_to_start_position_false();
    void set_angle_to_show_down_position_false();
    void set_angle_to_show_up_position_false();
    void set_angle_to_show_side_position_false();
    void callbackJoy(const sensor_msgs::Joy msg);
    void callBackArmCommand(const ohm_teleop_msgs::ArmCommand msg);

    void publish_to_inverse();

    bool get_initialize_joystick() const;
    bool get_space_constraints() const;
    bool get_set_angle_to_zero_position() const;
    bool get_set_angle_to_start_position() const;
    bool get_set_angle_to_show_down_position() const;
    bool get_set_angle_to_show_up_position() const;
    bool get_set_angle_to_show_side_position() const;
    std::vector<double> get_selfcollision_detection_position();

    unsigned int _nr_of_cmds   = 9;
    unsigned int _nr_of_joints = 7;
    unsigned int _nr_of_pos    = 3;

    //Part two
    void set_joystick_input_from_remap(ohm_teleop_msgs::ArmCommand joy_input);
    void set_zero_position_from_remap(ohm_teleop_msgs::ArmCommand init_zero_position);
    void set_start_position_from_remap(ohm_teleop_msgs::ArmCommand init_start_position);
    void set_show_down_from_remap(ohm_teleop_msgs::ArmCommand init_show_down_position);
    void set_show_up_from_remap(ohm_teleop_msgs::ArmCommand init_show_up_position);
    void set_show_side_from_remap(ohm_teleop_msgs::ArmCommand init_show_side_position);



private:

    std::vector<double> relative_rotation_change{0.0, 0.0, 0.0};
    std::vector<double> relative_position_change{0.0, 0.0, 0.0};
    std::vector<double> new_rotation_matrix{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

    const double linear_scale_position{0.0045};
    const double linear_scale_rotation{0.05};
    const double linear_scale_gripper{0.05};

    bool old_coordinate_system = false;
    bool change_coordinate_system = false;
    double coordiante_system{0.0};

    bool init_joy_to_move = false;
    bool signal_to_move_arm_in_real = false;
    bool change_angles_to_zero_position = false;
    bool change_angles_to_start_position = false;
    bool change_angles_to_show_down_position = false;
    bool change_angles_to_show_up_position = false;
    bool change_angles_to_show_side_position = false;


    bool reached_space_constrains = false;

    const double max_space_constrain = 1.22; //sphere around robot for maximum reach

    sensor_msgs::JointState _actual_joint_position;

    std::vector<double> selfcollision_max{0.450, 0.220, 0.05}; //use in get_selcollision_detection_position()
    std::vector<double> selfcollision_min{-0.300, -0.220, 0.0}; //use in get_selcollision_detection_position()

    ros::NodeHandle prvNh{"~"};

    std::string topicArmCommand;

    ros::NodeHandle mn;
    ros::Publisher _move_to_inverse;
    ros::Publisher _gripper_cs_active;
    ros::Publisher _arm_reached_space_constraints;
    ros::Subscriber _joint_angles_inverse;
    ros::Subscriber _standard_joy;
    ros::Subscriber _subsArmCommand;

    actionlib::SimpleActionClient<ohm_actors_msgs::ActionGripperAction>* _as;//("gripper_action", false);

};

#endif // MOVE_H

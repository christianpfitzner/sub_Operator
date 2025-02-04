#include "move.h"
#include <std_msgs/Bool.h>

Move::Move(){
  //_actual_joint_position.header.stamp = ros::Time::now();
  _actual_joint_position.header.frame_id = "";

  _actual_joint_position.name.push_back("joint_1");
  _actual_joint_position.position.push_back(0.0);
  _actual_joint_position.name.push_back("joint_2");
  _actual_joint_position.position.push_back(0.0);
  _actual_joint_position.name.push_back("joint_3");
  _actual_joint_position.position.push_back(M_PI);
  _actual_joint_position.name.push_back("joint_4");
  _actual_joint_position.position.push_back(-M_PI);
  _actual_joint_position.name.push_back("joint_5");
  _actual_joint_position.position.push_back(0.0);
  _actual_joint_position.name.push_back("joint_6");
  _actual_joint_position.position.push_back(0.0);
  _actual_joint_position.name.push_back("joint_7");
  _actual_joint_position.position.push_back(0.0);
  _actual_joint_position.name.push_back("gr1");
  _actual_joint_position.position.push_back(0.0);
  _actual_joint_position.name.push_back("gr2");
  _actual_joint_position.position.push_back(0.0);

  //Publisher
  std::string topicGripperCsActive;
  std::string topicArmReachedSpaceConstraints;
  _move_to_inverse = mn.advertise<ohm_move::RobotarmCtrl>("tcp/ctrl", 1, this);
  prvNh.param<std::string>("topic_gripper_cs_active", topicGripperCsActive, "inverse/gripper_cs_active");
  prvNh.param<std::string>("topic_arm_reached_space_constraints", topicArmReachedSpaceConstraints, "inverse/arm_reached_space_constraints");
  _gripper_cs_active = mn.advertise<std_msgs::Bool>(topicGripperCsActive, 1);
  _arm_reached_space_constraints = mn.advertise<std_msgs::Bool>(topicArmReachedSpaceConstraints, 1);

  //Subscriber - Update JointAngles from ohm_inverse_node
  _joint_angles_inverse = mn.subscribe("/joint_visual", 1, &Move::set_joint_angles, this);

  //Subscriber - Only for Simulation purposes or moving the manipulator without the robot
//  _standard_joy = mn.subscribe("joy", 1, &Move::callbackJoy, this);

  //Subscriber - Will be used if Schroedi and Manipulator are in use together


  prvNh.param<std::string>("topic_arm_command", topicArmCommand, "arm_command");
  _subsArmCommand = mn.subscribe(topicArmCommand, 1, &Move::callBackArmCommand, this);

  _as = new actionlib::SimpleActionClient<ohm_actors_msgs::ActionGripperAction>("pimmel", false);


}

Move::~Move()
{
  std::cout << __PRETTY_FUNCTION__ <<  " called destructor " << std::endl;
}

void Move::publish_to_inverse(){

  ohm_move::RobotarmCtrl  _current_cmd;

  _current_cmd.theta_old.header.stamp = ros::Time::now();
  _current_cmd.theta_old.header.frame_id = "";

  _current_cmd.theta_old.name = _actual_joint_position.name;
  _current_cmd.theta_old.position = _actual_joint_position.position;

  _current_cmd.drei_rot.data = new_rotation_matrix;
  _current_cmd.des_pos.data = relative_position_change;
  _current_cmd.gripper_cmd = _actual_joint_position.position.at(7);
  _current_cmd.amr_config = 0;
  if (signal_to_move_arm_in_real == true) {_current_cmd.amr_config = 1;} // has to be true that the arm moves in real

  //Publisher
  _move_to_inverse.publish(_current_cmd);


}

void Move::callbackJoy(const sensor_msgs::Joy msg) {

  // Initialize Joystick
  this->set_initialize_joystick(msg);
  // Check if Joystick is initialized
  if (this->get_initialize_joystick() == false)
  {
    ROS_WARN("Joystick not initalized! Press L2 und R2 together for 1 second");
    return ;
  }

  this->set_joystick_input(msg); // Joystick Input - Coordinate Fram and Change of Pose or Gripper
  this->set_new_goal_position(); // Calculate new Position and Orientation Matrix

  //Check for zero angles position
  this->set_zero_position(msg);
  if (this->get_set_angle_to_zero_position() == true) {this->set_angle_to_zero_positon_false();}

  // Check for start angles position
  this->set_start_position(msg);
  if (this->get_set_angle_to_start_position() == true) {this->set_angle_to_start_position_false();}

  // Check for show down position
  this->set_show_down_position(msg);
  if (this->get_set_angle_to_show_down_position() == true) {this->set_angle_to_show_down_position_false();}

  // Check for show up position
  this->set_show_up_position(msg);
  if (this->get_set_angle_to_show_up_position() == true) {this->set_angle_to_show_up_position_false();}

  // Check for show side position
  this->set_show_side_position(msg);
  if (this->get_set_angle_to_show_side_position() == true) {this->set_angle_to_show_side_position_false();}

  // Collision Detection - Self Collision and Space Constraints
  this->get_selfcollision_detection_position();
  this->set_check_space_constraints();
  if (this->get_space_constraints() == true){return;}

  // Publish to ohm_inverse_node
  this->publish_to_inverse();

}


void Move::callBackArmCommand(const ohm_teleop_msgs::ArmCommand msg)
{

  /*ohm_actors_msgs::ActionGripperGoal goal;
  if(msg.grauf)
  {
    goal.cmd = ohm_actors_msgs::ActionGripperGoal::OPEN;
    _as->sendGoal(goal);
  }
  else if(msg.grzu)
  {
    goal.cmd = ohm_actors_msgs::ActionGripperGoal::CLOSE;
    _as->sendGoal(goal);
  }*/


this->set_joystick_input_from_remap(msg); // Joystick Input - Coordinate Fram and Change of Pose or Gripper
this->set_new_goal_position(); // Calculate new Position and Orientation Matrix

//Check for zero angles position
this->set_zero_position_from_remap(msg);
if (this->get_set_angle_to_zero_position() == true) {this->set_angle_to_zero_positon_false();}

// Check for start angles position
this->set_start_position_from_remap(msg);
if (this->get_set_angle_to_start_position() == true) {this->set_angle_to_start_position_false();}

// Check for show down angles position
this->set_show_down_from_remap(msg);
if (this->get_set_angle_to_show_down_position() == true) {this->set_angle_to_show_down_position_false();}

// Check for show up position
this->set_show_up_from_remap(msg);
if (this->get_set_angle_to_show_up_position() == true) {this->set_angle_to_show_up_position_false();}

// Check for show side position
this->set_show_side_from_remap(msg);
if (this->get_set_angle_to_show_side_position() == true) {this->set_angle_to_show_side_position_false();}


// Collision Detection - Self Collision and Space Constraints
this->get_selfcollision_detection_position();
this->set_check_space_constraints();
//if (this->get_space_constraints() == true){return;}
  std_msgs::Bool armReachedSpaceConstraints;
  double space_maximum = std::sqrt(pow(relative_position_change.at(0), 2.0) + pow(relative_position_change.at(1), 2.0) + pow(relative_position_change.at(2), 2.0));
  if (space_maximum >= 1.0)
    armReachedSpaceConstraints.data = true;
  else
    armReachedSpaceConstraints.data = false;
  _arm_reached_space_constraints.publish(armReachedSpaceConstraints);
// Publish to ohm_inverse_node
this->publish_to_inverse();
}


void Move::set_joint_angles(sensor_msgs::JointState js_update)
{
  _actual_joint_position.position.clear();

  _actual_joint_position.header.stamp = ros::Time::now();
  _actual_joint_position.header.frame_id = "";

  _actual_joint_position.position.push_back(js_update.position.at(0));
  _actual_joint_position.position.push_back(-js_update.position.at(1));
  _actual_joint_position.position.push_back(-js_update.position.at(2));
  _actual_joint_position.position.push_back(js_update.position.at(3)-M_PI); // gegen minus unendlich -js_update.position.at(3)-M_PI
  _actual_joint_position.position.push_back(js_update.position.at(4));
  _actual_joint_position.position.push_back(js_update.position.at(5));
  _actual_joint_position.position.push_back(js_update.position.at(6));
  //Correction angle of joint7 - because of atan2 in ohm_inverse node
  if (_actual_joint_position.position.at(6) > M_PI)
  {
    _actual_joint_position.position.at(6) = _actual_joint_position.position.at(6) - 2*M_PI;
  }
  _actual_joint_position.position.push_back(js_update.position.at(7));
  _actual_joint_position.position.push_back(js_update.position.at(8));
}


void Move::set_joystick_input(sensor_msgs::Joy joy_input){

  if (joy_input.buttons[8] == 1 && !old_coordinate_system)
  {
    change_coordinate_system = !change_coordinate_system;
  }
  old_coordinate_system = joy_input.buttons[8];


  if (change_coordinate_system==true)
  {
    coordiante_system = 1.0;
  }

  if (change_coordinate_system==false)
  {
    coordiante_system = 0.0;
  }

  //Check for coordinate system and then change the input of axes for global and local coordinate frame
  if (coordiante_system == 0.0)
  {
    relative_position_change[0] = joy_input.axes[1]*linear_scale_position;
    relative_position_change[2] = joy_input.axes[4]*linear_scale_position;
    relative_position_change[1] = joy_input.axes[0]*linear_scale_position;
  }

  if (coordiante_system == 1.0)
  {
    relative_position_change[0] = -joy_input.axes[4]*linear_scale_position;
    relative_position_change[2] = joy_input.axes[1]*linear_scale_position;
    relative_position_change[1] = joy_input.axes[0]*linear_scale_position;
  }


  relative_rotation_change[0] = (joy_input.buttons[7] -joy_input.buttons[6])*linear_scale_rotation;
  relative_rotation_change[1] = (joy_input.buttons[4] -joy_input.buttons[5])*linear_scale_rotation;
  relative_rotation_change[2] = (joy_input.buttons[15] -joy_input.buttons[16])*linear_scale_rotation;


  _actual_joint_position.position.at(7) += (joy_input.buttons[1]-joy_input.buttons[3]) * linear_scale_gripper;

}

void Move::set_initialize_joystick(sensor_msgs::Joy init_joystick)
{
  const double axisL2 = init_joystick.axes[2];
  const double axisR2 = init_joystick.axes[5];

  if (axisL2 == -1.0 && axisR2 == -1.0 )
  {
    init_joy_to_move = true;
  }

}

void Move::set_zero_position(sensor_msgs::Joy init_zero_position)
{

  if (init_zero_position.buttons[9] == 1)
  {
    signal_to_move_arm_in_real = true; //As long as variable is true - publisher to arm in ohm_inverse_node is on
    change_angles_to_zero_position = true;
    relative_position_change = {0.377, 0.0, 0.0};
    new_rotation_matrix = {0.0, 0.0, -1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0 };
    _actual_joint_position.position.at(0) = 0.0;
    _actual_joint_position.position.at(1) = 0.0;
    _actual_joint_position.position.at(2) = M_PI;
    _actual_joint_position.position.at(3) =-M_PI;
    _actual_joint_position.position.at(4) = 0.0;
    _actual_joint_position.position.at(5) = 0.0;
    _actual_joint_position.position.at(6) = 0.0;
    _actual_joint_position.position.at(7) = 0.0;
    _actual_joint_position.position.at(8) = 0.0;
  }

}

void Move::set_start_position(sensor_msgs::Joy init_start_position)
{

  if (init_start_position.buttons[13] == 1)
  {

    change_angles_to_start_position = true;
    relative_position_change = {0.377, 0.0, 0.4};
    new_rotation_matrix = {0.0, 0.0, -1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0 };
    _actual_joint_position.position.at(0) = 0.0;
    _actual_joint_position.position.at(1) = 0.004;
    _actual_joint_position.position.at(2) = 2.688;
    _actual_joint_position.position.at(3) =-2.255;
    _actual_joint_position.position.at(4) = 0.0;
    _actual_joint_position.position.at(5) = 0.474;
    _actual_joint_position.position.at(6) = 0.0;
    _actual_joint_position.position.at(7) = 0.0;
    _actual_joint_position.position.at(8) = 0.0;
  }
}


void Move::set_show_down_position(sensor_msgs::Joy init_show_down_position)
{

  if (init_show_down_position.buttons[14] == 1)
  {

    change_angles_to_show_down_position = true;
    relative_position_change = {0.8053, 0.0, 0.4357};
    new_rotation_matrix = {-1.0, 0.02, -0.001, 0.02, 1.0, 0.0, 0.001, 0.0, -1.0 };
    _actual_joint_position.position.at(0) = 0.0;
    _actual_joint_position.position.at(1) = 0.1631;
    _actual_joint_position.position.at(2) = 1.4438;
    _actual_joint_position.position.at(3) =-1.3457;
    _actual_joint_position.position.at(4) = 0.0;
    _actual_joint_position.position.at(5) = 1.491;
    _actual_joint_position.position.at(6) = 0.0;
    _actual_joint_position.position.at(7) = 0.0;
    _actual_joint_position.position.at(8) = 0.0;
  }
}

void Move::set_show_up_position(sensor_msgs::Joy init_show_up_position)
{

  if (init_show_up_position.buttons[2] == 1)
  {

    change_angles_to_show_up_position = true;
    relative_position_change = {0.3748, 0.0, 0.1751};
    new_rotation_matrix = {0.886, -0.02, -0.4627, 0.021, 1.0, 0.0, 0.4626, -0.006, 0.8865 };
    _actual_joint_position.position.at(0) = 0.0;
    _actual_joint_position.position.at(1) = 0.031;
    _actual_joint_position.position.at(2) = 2.9765;
    _actual_joint_position.position.at(3) =-2.726;
    _actual_joint_position.position.at(4) = 3.13;
    _actual_joint_position.position.at(5) = 0.871;
    _actual_joint_position.position.at(6) = 3.11;
    _actual_joint_position.position.at(7) = 0.0;
    _actual_joint_position.position.at(8) = 0.0;
  }
}

void Move::set_show_side_position(sensor_msgs::Joy init_show_side_position)
{

  if (init_show_side_position.buttons[0] == 1)
  {

    change_angles_to_show_side_position = true;
    relative_position_change = {0.5052, 0.0, 0.2146};
    new_rotation_matrix = {-0.01, 0.07, -1.0, 0.9498, 0.3127, 0.012, 0.3128, -0.9473, -0.0693 };
    _actual_joint_position.position.at(0) = 0.0;
    _actual_joint_position.position.at(1) = 0.049;
    _actual_joint_position.position.at(2) = 2.25;
    _actual_joint_position.position.at(3) =-2.63;
    _actual_joint_position.position.at(4) = 1.61;
    _actual_joint_position.position.at(5) = 1.25;
    _actual_joint_position.position.at(6) = 1.91;
    _actual_joint_position.position.at(7) = 0.0;
    _actual_joint_position.position.at(8) = 0.0;
  }
}


bool Move::get_initialize_joystick() const  { return init_joy_to_move;  }


bool Move::get_set_angle_to_zero_position() const {return change_angles_to_zero_position;}

bool Move::get_set_angle_to_start_position() const {return change_angles_to_start_position;}

bool Move::get_set_angle_to_show_down_position() const {return change_angles_to_show_down_position;}

bool Move::get_set_angle_to_show_up_position() const {return change_angles_to_show_up_position;}

bool Move::get_set_angle_to_show_side_position() const {return change_angles_to_show_side_position;}

bool Move::get_space_constraints() const {return reached_space_constrains;}

std::vector<double> Move::get_selfcollision_detection_position()
{
  if(	((relative_position_change.at(0) < selfcollision_max.at(0) ) && (relative_position_change.at(0) > selfcollision_min.at(0))) && ((relative_position_change.at(1)<selfcollision_max.at(1)) && (relative_position_change.at(1)>selfcollision_min.at(1))))
  {
    if(relative_position_change.at(2) < 0.0)
    {
      relative_position_change.at(2) = 0.0;
      ROS_ERROR_STREAM("Be Careful - Self Collision - Type 1");
    }
  }

  if (((relative_position_change.at(0) < selfcollision_max.at(0)) && (relative_position_change.at(0) > selfcollision_min.at(0))) && ((relative_position_change.at(2) < selfcollision_max.at(2)) && (relative_position_change.at(2) >= selfcollision_min.at(2))))
  {
    relative_position_change.at(0) = 0.377;
    ROS_ERROR_STREAM("Be Careful - Self Collision - Type 2");
  }

  if (((relative_position_change.at(0) < selfcollision_max.at(0)) && (relative_position_change.at(0) > selfcollision_min.at(0))) && ((relative_position_change.at(2) < selfcollision_max.at(2)) && (relative_position_change.at(2) >= selfcollision_min.at(2))) && ((relative_position_change.at(1)<selfcollision_max.at(1)) && (relative_position_change.at(1)>selfcollision_min.at(1))))
  {
    relative_position_change.at(1) = 0.0;
    ROS_ERROR_STREAM("Be Careful - Self Collision - Type 3");
  }

  return relative_position_change;
}

void Move::set_check_space_constraints()
{
  double space_maximum = 0.0;
  space_maximum = std::sqrt(pow(relative_position_change.at(0), 2.0) + pow(relative_position_change.at(1), 2.0) + pow(relative_position_change.at(2), 2.0));

  if (space_maximum >= max_space_constrain) {
    ROS_ERROR(" Out of reach! - Reached Space Constraints");
    reached_space_constrains = true;
  }
}

void Move::set_angle_to_zero_positon_false()
{
  change_angles_to_zero_position = false;
}

void Move::set_angle_to_start_position_false()
{
  change_angles_to_start_position = false;
}

void Move::set_angle_to_show_down_position_false()
{
  change_angles_to_show_down_position = false;
}

void Move::set_angle_to_show_up_position_false()
{
  change_angles_to_show_up_position = false;
}

void Move::set_angle_to_show_side_position_false()
{
  change_angles_to_show_side_position = false;
}

void Move::set_new_goal_position(){

  double convert_joints[7]    = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  double convert_pd[3]        = {0.0, 0.0, 0.0};
  double convert_rd[3]        = {0.0, 0.0, 0.0};
  double convert_rd_3x3[9]    = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

  std::copy(relative_position_change.begin(), relative_position_change.end(), convert_pd);
  std::copy(relative_rotation_change.begin(), relative_rotation_change.end(), convert_rd);
  std::copy(_actual_joint_position.position.begin(), _actual_joint_position.position.end()-2, convert_joints);

  Aktuelle_Position(convert_joints, convert_rd, convert_pd, coordiante_system, convert_rd_3x3);

  for (size_t i = 0; i < _nr_of_cmds; ++i){
    new_rotation_matrix.at(i) = convert_rd_3x3[i];
  }

  for (size_t j = 0; j < _nr_of_pos; ++j){
    relative_position_change.at(j) = convert_pd[j];
  }

}

//Part 2

void Move::set_joystick_input_from_remap(ohm_teleop_msgs::ArmCommand joy_input){

  if (joy_input.change_coordinate_system == 1 && !old_coordinate_system)
  {
    change_coordinate_system = !change_coordinate_system;
  }
  old_coordinate_system = joy_input.change_coordinate_system;


  if (change_coordinate_system==true)
  {
    coordiante_system = 1.0;
  }

  if (change_coordinate_system==false)
  {
    coordiante_system = 0.0;
  }

  //Check for coordinate system and then change the input of axes for global and local coordinate frame
  std_msgs::Bool gripperCsActive;
  if (coordiante_system < 0.1)
  {
    relative_position_change[0] = joy_input.xe*linear_scale_position;
    relative_position_change[2] = joy_input.ze*linear_scale_position;
    relative_position_change[1] = joy_input.ye*linear_scale_position;
    gripperCsActive.data = false;
  }
  else// (coordiante_system == 1.0)
  {
    relative_position_change[0] = -joy_input.ze*linear_scale_position;
    relative_position_change[2] = joy_input.xe*linear_scale_position;
    relative_position_change[1] = joy_input.ye*linear_scale_position;
    gripperCsActive.data = true;
  }


  relative_rotation_change[0] = (joy_input.rplus -joy_input.rminus)*linear_scale_rotation;
  relative_rotation_change[1] = (joy_input.pplus -joy_input.pminus)*linear_scale_rotation / 6.0;
  relative_rotation_change[2] = (joy_input.yplus -joy_input.yminus)*linear_scale_rotation / 6.0;


  // Probably without use
 if(_actual_joint_position.position.at(7) < -1.6)
    _actual_joint_position.position.at(7) = -1.6;
  else if(_actual_joint_position.position.at(7) > 1.0)
    _actual_joint_position.position.at(7) = 1.0;
  else
    _actual_joint_position.position.at(7) += (joy_input.grauf-joy_input.grzu) * linear_scale_gripper;
 //  ROS_ERROR("Gripper des val %f \n", _actual_joint_position.position.at(7));   
 //  std::cout << "gripper des val "  <<_actual_joint_position.position.at(7) << std::endl;
// std_msgs::Bool armReachedSpaceConstraints;
// armReachedSpaceConstraints.data = reached_space_constrains;
 //_arm_reached_space_constraints.publish(armReachedSpaceConstraints);
  _gripper_cs_active.publish(gripperCsActive);

}

void Move::set_zero_position_from_remap(ohm_teleop_msgs::ArmCommand init_zero_position)
{

  if (init_zero_position.initial_pose == 1)
  {
    signal_to_move_arm_in_real = true; //As long as variable is true - publisher to arm in ohm_inverse_node is on
    change_angles_to_zero_position = true;
    relative_position_change = {0.377, 0.0, 0.0};
    new_rotation_matrix = {0.0, 0.0, -1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0 };
    _actual_joint_position.position.at(0) = 0.0;
    _actual_joint_position.position.at(1) = 0.0;
    _actual_joint_position.position.at(2) = M_PI;
    _actual_joint_position.position.at(3) =-M_PI;
    _actual_joint_position.position.at(4) = 0.0;
    _actual_joint_position.position.at(5) = 0.0;
    _actual_joint_position.position.at(6) = 0.0;
    _actual_joint_position.position.at(7) = 0.0;
    _actual_joint_position.position.at(8) = 0.0;
  }

}


void Move::set_start_position_from_remap(ohm_teleop_msgs::ArmCommand init_start_position)
{

  if (init_start_position.initial_up == 1)
  {
      change_angles_to_start_position = true;
      relative_position_change = {0.377, 0.0, 0.4};
      new_rotation_matrix = {0.0, 0.0, -1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0 };
      _actual_joint_position.position.at(0) = 0.0;
      _actual_joint_position.position.at(1) = 0.004;
      _actual_joint_position.position.at(2) = 2.688;
      _actual_joint_position.position.at(3) =-2.255;
      _actual_joint_position.position.at(4) = 0.0;
      _actual_joint_position.position.at(5) = 0.474;
      _actual_joint_position.position.at(6) = 0.0;
      _actual_joint_position.position.at(7) = 0.0;
      _actual_joint_position.position.at(8) = 0.0;
  }

}

void Move::set_show_down_from_remap(ohm_teleop_msgs::ArmCommand init_show_down_position)
{
  if(init_show_down_position.initial_look_down == 1)
  {
    change_angles_to_show_down_position = true;
    relative_position_change = {0.8053, 0.0, 0.4357};
    new_rotation_matrix = {-1.0, 0.02, -0.001, 0.02, 1.0, 0.0, 0.001, 0.0, -1.0 };
    _actual_joint_position.position.at(0) = 0.0;
    _actual_joint_position.position.at(1) = 0.1631;
    _actual_joint_position.position.at(2) = 1.4438;
    _actual_joint_position.position.at(3) =-1.3457;
    _actual_joint_position.position.at(4) = 0.0;
    _actual_joint_position.position.at(5) = 1.491;
    _actual_joint_position.position.at(6) = 0.0;
    _actual_joint_position.position.at(7) = 0.0;
    _actual_joint_position.position.at(8) = 0.0;
  }
}

void Move::set_show_up_from_remap(ohm_teleop_msgs::ArmCommand init_show_up_position)
{

  if (init_show_up_position.initial_tunnel_up == 1)
  {

      change_angles_to_show_up_position = true;
      relative_position_change = {0.3748, 0.0, 0.1751};
      new_rotation_matrix = {0.886, -0.02, -0.4627, 0.021, 1.0, 0.0, 0.4626, -0.006, 0.8865 };
      _actual_joint_position.position.at(0) = 0.0;
      _actual_joint_position.position.at(1) = -0.031;
      _actual_joint_position.position.at(2) = 2.9765;
      _actual_joint_position.position.at(3) =-2.726;
      _actual_joint_position.position.at(4) =-3.13;
      _actual_joint_position.position.at(5) = 0.871;
      _actual_joint_position.position.at(6) = 3.11;
      _actual_joint_position.position.at(7) = 0.0;
      _actual_joint_position.position.at(8) = 0.0;
  }
}

void Move::set_show_side_from_remap(ohm_teleop_msgs::ArmCommand init_show_side_position)
{

  if (init_show_side_position.initial_tunnel_side == 1)
  {

      change_angles_to_show_side_position = true;
      relative_position_change = {0.5052, 0.0, 0.2146};
      new_rotation_matrix = {-0.01, 0.07, -1.0, 0.9498, 0.3127, 0.012, 0.3128, -0.9473, -0.0693 };
      _actual_joint_position.position.at(0) = 0.0;
      _actual_joint_position.position.at(1) = 0.049;
      _actual_joint_position.position.at(2) = 2.25;
      _actual_joint_position.position.at(3) =-2.63;
      _actual_joint_position.position.at(4) = 1.61;
      _actual_joint_position.position.at(5) = 1.25;
      _actual_joint_position.position.at(6) = 1.91;
      _actual_joint_position.position.at(7) = 0.0;
      _actual_joint_position.position.at(8) = 0.0;
  }
}



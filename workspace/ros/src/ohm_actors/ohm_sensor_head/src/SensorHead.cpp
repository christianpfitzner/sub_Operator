#include "SensorHead.h"

#include <std_msgs/Float64.h>
#include <std_msgs/Float32.h>
#include <ohm_actors_msgs/SetParameter.h>
#include <tf/tf.h>

#include <iostream>
#include <cmath>

#include <Eigen/Geometry>

SensorHead::SensorHead(ros::NodeHandle& nh)
: _nh(nh),
  _pubCommand(CountServo),
  _srvSpeed(CountServo),
  _pubSpeed(CountServo),
  _srvTorque(CountServo),
  _subServo(CountServo),
  _targetTopic(CountMode),
  _tfServoSource(CountServo),
  _tfServoTarget(CountServo),
  _mode(ohm_actors_msgs::SensorHeadMode::Request::NONE),
  _servoStatus(CountServo),
  _servoUpdate(CountServo, false)
{
  ros::NodeHandle para("~");
  std::string topic;
  std::string topicSrvSetHoming;

  para.param<std::string>("topic_command_pitch", topic, "tilt/pos/des");
  _pubCommand[Pitch] = _nh.advertise<std_msgs::Float32>(topic, 2);
  para.param<std::string>("topic_command_yaw"  , topic, "pan/pos/des");
  _pubCommand[Yaw]   = _nh.advertise<std_msgs::Float32>(topic, 2);

  para.param<std::string>("service_speed_pitch", topic, "tilt/speed/des");
  _srvSpeed[Pitch] = _nh.serviceClient<ohm_actors_msgs::SetParameter>(topic);
  _pubSpeed[Pitch] = _nh.advertise<std_msgs::Float32>(topic, 2);
  para.param<std::string>("service_speed_yaw"  , topic, "pan/speed/des");
  _srvSpeed[Yaw]   = _nh.serviceClient<ohm_actors_msgs::SetParameter>(topic);
  _pubSpeed[Yaw] = _nh.advertise<std_msgs::Float32>(topic, 2);

  para.param<std::string>("service_torque_pitch", topic, "sensor_head/pitch/torque");
  _srvTorque[Pitch] = _nh.serviceClient<ohm_actors_msgs::SetParameter>(topic);
  para.param<std::string>("service_torque_yaw"  , topic, "sensor_head/yaw/torque");
  _srvTorque[Yaw]   = _nh.serviceClient<ohm_actors_msgs::SetParameter>(topic);

  para.param<std::string>("topic_status_pitch", topic, "sensor_head/pitch/status");
  _subServo[Pitch] = _nh.subscribe(topic, 2, &SensorHead::callbackStatusServoPitch, this);
  para.param<std::string>("topic_status_yaw"  , topic, "sensor_head/yaw/status");
  _subServo[Yaw]   = _nh.subscribe(topic, 2, &SensorHead::callbackStatusServoYaw  , this);

  para.param<std::string>("joy_topic", topic, "sensorhead_joy");
  //    _joy_sensorhead_sub = _nh.subscribe(topic, 2, &SensorHead::callbackJoy, this);

  para.param<std::string>("topic_target_joy"      , _targetTopic[ControlledByJoy]   , "sensorhead_joy"  );
  para.param<std::string>("topic_target_point"    , _targetTopic[LookAtPoint]       , "goal/look_at"    );
  para.param<std::string>("topic_target_oculus"   , _targetTopic[ControlledByOculus], "pose/oculus"     );
  para.param<std::string>("topic_target_direction", _targetTopic[BindDirection]     , "goal/sensor_head");

  para.param<std::string>("tf_source"      , _tfSource            , "map"                      );
  para.param<std::string>("tf_target"      , _tfTarget            , "base_footprint"           );
  para.param<std::string>("tf_yaw_source"  , _tfServoSource[Yaw]  , "sensor_head/yaw/base"     );
  para.param<std::string>("tf_yaw_target"  , _tfServoTarget[Yaw]  , "sensor_head/yaw/rotated"  );
  para.param<std::string>("tf_pitch_source", _tfServoSource[Pitch], "sensor_head/pitch/base"   );
  para.param<std::string>("tf_pitch_target", _tfServoTarget[Pitch], "sensor_head/pitch/rotated");
  para.param<std::string>("tf_sensor"      , _tfSensor            , "sensor_head/xtion"        );
  para.param<std::string>("topic_srv_set_homing", topicSrvSetHoming, "set_homing");

  para.param<std::string>("service_mode", topic, "sensor_head/mode");
  _srvMode = _nh.advertiseService(topic, &SensorHead::callbackMode, this);
  _srvSetHoming = _nh.advertiseService(topicSrvSetHoming, &SensorHead::callBackSrvSerHoming, this);

  para.param<double>("max_motion_speed", _maxSpeed, 1.0f);
  _directionYaw = 0.0;

  para.param<double>("homing_yaw",       _homing_yaw,   0.0f);
  para.param<double>("homing_pitch",     _homing_pitch, 0.75f);

  _mode = ControlledByJoy;

  _joy_sensorhead_sub = _nh.subscribe("sensorhead_joy", 2, &SensorHead::callbackOhmJoy, this);

  _timerMain = _nh.createTimer(50.0, &SensorHead::callBackTimerMain, this);

}

void SensorHead::spinOnce(void)
{
  switch (_mode)
  {
  case ohm_actors_msgs::SensorHeadMode::Request::BIND_DIRECTION:
  {
    tf::StampedTransform transform;

    try
    {
      const ros::Time stamp(ros::Time::now());
      _listener.waitForTransform(_tfSource, _tfTarget, stamp, ros::Duration(1.0));
      _listener.lookupTransform(_tfSource, _tfTarget, stamp, transform);
    }
    catch(tf::TransformException& ex)
    {
      ROS_ERROR("%s", ex.what());
      break;
    }

    const Eigen::Quaternionf quat(transform.getRotation().w(), transform.getRotation().x(),
        transform.getRotation().y(), transform.getRotation().z());
    //	    const Eigen::Matrix3f Rmt(Eigen::AngleAxisf(_directionYaw, Eigen::Vector3f::UnitZ()));
    const Eigen::Matrix3f Rmt(_Rmt);
    Eigen::Matrix3f Rmr(quat.matrix());
    float yaw = 0.0f;
    float pitch = 0.0f;

    {
      const Eigen::Matrix3f Rrt(Rmr.inverse() * Rmt);

      Eigen::Vector3f vrtx(Rrt * Eigen::Vector3f::UnitX());
      vrtx.z() = 0.0f;
      yaw = std::acos(vrtx.x() / vrtx.norm());

      /* atan2 ??? */
      if (vrtx.y() < 0.0f)
        yaw *= -1.0f;
    }

    {
      Rmr *= Eigen::Matrix3f(Eigen::AngleAxisf(yaw, Eigen::Vector3f::UnitZ()));
      const Eigen::Matrix3f Rrt(Rmr.inverse() * Rmt);

      Eigen::Vector3f vrtz(Rrt * Eigen::Vector3f::UnitZ());
      vrtz.y() = 0.0f;
      pitch = std::acos(vrtz.z() / vrtz.norm());

      /* atan2 ??? */
      if (vrtz.x() < 0.0f)
        pitch *= -1.0f;
    }

    std_msgs::Float64 command;
    command.data = yaw;
    _pubCommand[Yaw].publish(command);

    command.data = pitch;
    _pubCommand[Pitch].publish(command);
  }
  break;

  case ohm_actors_msgs::SensorHeadMode::Request::LOOK_AT_POINT:
  {
    tf::StampedTransform transform;

    try
    {
      //                const ros::Time stamp(ros::Time::now());
      //                _listener.waitForTransform(_tfSource, _tfSensor, stamp, ros::Duration(1.0));
      _listener.lookupTransform(_tfSensor,_tfSource ,ros::Time(0), transform);
    }
    catch(tf::TransformException& ex)
    {
      ROS_ERROR("%s", ex.what());
      break;
    }


    /* Calculate the yaw angle. */
    Eigen::Vector4f p(_lookAtPoint.x, _lookAtPoint.y, _lookAtPoint.z, 1.0f);
    Eigen::Matrix4f Tmr(Eigen::Matrix4f::Identity());
    std_msgs::Float32 command;

    Tmr(0, 3) = transform.getOrigin().x();
    Tmr(1, 3) = transform.getOrigin().y();
    Tmr(2, 3) = transform.getOrigin().z();
    Tmr.block<3, 3>(0, 0) = Eigen::Quaternionf(transform.getRotation().w(),
        transform.getRotation().x(),
        transform.getRotation().y(),
        transform.getRotation().z()).matrix();

    Eigen::Vector4f p_r = Tmr.inverse() * p;
    p_r.z() = 0.0f;
    p_r.w() = 0.0f;
    command.data = std::acos(p_r.x() / p_r.norm());

    if (p_r.y() < 0.0f)
      command.data *= -1.0f;

    _pubCommand[Yaw].publish(command);


    /* Calculate the pitch angle. */
    Eigen::Matrix4f Tyaw(Eigen::Matrix4f::Identity());

    Tyaw.block<3, 3>(0, 0) = Eigen::AngleAxisf(command.data, Eigen::Vector3f::UnitZ()).matrix();
    Tmr = Tmr * Tyaw;
    p_r = Tmr.inverse() * p;
    p_r.y() = 0.0f;
    p_r.w() = 0.0f;
    command.data = std::acos(p_r.x() / p_r.norm());

    if (p_r.z() > 0.0f)
      command.data *= -1.0f;

    _pubCommand[Pitch].publish(command);
  }
  break;

  case ohm_actors_msgs::SensorHeadMode::Request::CONTROLED_BY_OCULUS:
    break;

  case ohm_actors_msgs::SensorHeadMode::Request::CONTROLED_BY_JOY:
    break;

  default:
  {
    std_msgs::Float64 command;
    command.data = 0.0;
    _pubCommand[Yaw].publish(command);
    _pubCommand[Pitch].publish(command);
  }
  break;
  }


  /* Publish current sensor head transforms to tf. */
//  tf::Transform transform;
//  tf::Quaternion rotation;
//
//
//  rotation.setRPY(0.0, 0.0, _servoStatus[Yaw].angle);
//  transform.setRotation(rotation);
//  _broadcaster.sendTransform(tf::StampedTransform(transform,
//      ros::Time::now(),
//      _tfServoSource[Yaw],
//      _tfServoTarget[Yaw]));
//
//  rotation.setRPY(0.0, _servoStatus[Pitch].angle, 0.0);
//  transform.setRotation(rotation);
//  _broadcaster.sendTransform(tf::StampedTransform(transform,
//      ros::Time::now(),
//      _tfServoSource[Pitch],
//      _tfServoTarget[Pitch]));

  ros::spinOnce();
}

bool SensorHead::callbackMode(ohm_actors_msgs::SensorHeadMode::Request& req,
    ohm_actors_msgs::SensorHeadMode::Response& res)
{
  if (req.mode == _mode)
    return true;


  /* Shutdown uneeded subscribers and publishers. */
  switch (_mode)
  {
  case ohm_actors_msgs::SensorHeadMode::Request::CONTROLED_BY_JOY:
    _subTarget.shutdown();
    break;

  case ohm_actors_msgs::SensorHeadMode::Request::LOOK_AT_POINT:
    _subTarget.shutdown();
    _servoUpdate[Yaw] = false;
    _servoUpdate[Pitch] = false;
    break;

  case ohm_actors_msgs::SensorHeadMode::Request::BIND_DIRECTION:
    _subTarget.shutdown();
    _servoUpdate[Yaw] = false;
    _servoUpdate[Pitch] = false;
    break;

  case ohm_actors_msgs::SensorHeadMode::Request::CONTROLED_BY_OCULUS:
    _servoUpdate[Yaw] = false;
    _servoUpdate[Pitch] = false;
    _subTarget.shutdown();
    break;

  case ohm_actors_msgs::SensorHeadMode::Request::NONE:
    _subTarget.shutdown();
    break;

  default:
    ROS_ERROR("Unkown mode. Return with false from service call.");
    return false;
  }


  /* Subscribe and publish needed topics. */
  switch (req.mode)
  {
  case ohm_actors_msgs::SensorHeadMode::Request::CONTROLED_BY_JOY:
    _subTarget = _nh.subscribe(_targetTopic[ControlledByJoy], 2, &SensorHead::callbackJoy, this);
    //       _subTarget = _nh.subscribe(_targetTopic[ControlledByJoy], 2, &SensorHead::callbackOhmJoy, this);
    break;

  case ohm_actors_msgs::SensorHeadMode::Request::LOOK_AT_POINT:
    _subTarget = _nh.subscribe(_targetTopic[LookAtPoint], 2, &SensorHead::callbackLookAtPoint, this);
    _servoUpdate[Yaw] = true;
    _servoUpdate[Pitch] = true;
    break;

  case ohm_actors_msgs::SensorHeadMode::Request::BIND_DIRECTION:
    _subTarget = _nh.subscribe(_targetTopic[BindDirection], 2, &SensorHead::callbackDirection, this);
    _servoUpdate[Yaw] = true;
    _servoUpdate[Pitch] = true;
    break;

  case ohm_actors_msgs::SensorHeadMode::Request::CONTROLED_BY_OCULUS:
    _subTarget = _nh.subscribe(_targetTopic[ControlledByOculus], 2, &SensorHead::callbackOculus, this);
    break;

  case ohm_actors_msgs::SensorHeadMode::Request::NONE:
    _servoUpdate[Yaw] = true;
    _servoUpdate[Pitch] = true;
    break;

  default:
    ROS_ERROR("Unkown mode. Return with false from service call.");
    return false;
  };

  _mode = req.mode;
  res.mode = _mode;
  ROS_INFO_STREAM("changed mode to " << static_cast<int>(_mode));

  return true;
}

void SensorHead::callbackStatusServoPitch(const std_msgs::Float32& msg)
{
  //if (_servoUpdate[Pitch]) _servoStatus[Pitch] = msg;
  _anglePitch = msg;
}

void SensorHead::callbackStatusServoYaw(const std_msgs::Float32& msg)
{
  _angleYaw = msg;
}

void SensorHead::callbackJoy(const sensor_msgs::Joy& msg)
{
  ROS_ERROR("%s - Dont use this method. Joy axes have been changed", __PRETTY_FUNCTION__);
  abort();


  //  const unsigned int X_AXIS = 2;
  //  const unsigned int Y_AXIS = 3;
  //  const float MIN_SPEED = 0.114f;
  //
  //  std_msgs::Float64 command;
  //  ohm_actors_msgs::SetParameter speed;
  //
  //  /* Left and right. */
  //  if (msg.axes[X_AXIS] < 0.0f)
  //  {
  //    command.data = -M_PI * 0.95f;
  //    speed.request.value = _maxSpeed * std::abs(msg.axes[X_AXIS]);
  //    _servoUpdate[Yaw] = true;
  //  }
  //  else if (msg.axes[X_AXIS] > 0.0f)
  //  {
  //    command.data = M_PI * 0.95f;
  //    speed.request.value = _maxSpeed * std::abs(msg.axes[X_AXIS]);
  //    _servoUpdate[Yaw] = true;
  //  }
  //  else
  //  {
  //    command.data = _servoStatus[Yaw].angle;
  //    speed.request.value = MIN_SPEED;
  //    _servoUpdate[Yaw] = false;
  //  }
  //
  //  if (speed.request.value < MIN_SPEED)
  //    speed.request.value = MIN_SPEED;
  //
  //  _srvSpeed[Yaw].call(speed);
  //  //    _pubCommand[Yaw].publish(command);
  //
  //  /* Up and down. */
  //  if (msg.axes[Y_AXIS] < 0.0f)
  //  {
  //    command.data = -M_PI * 0.5f;
  //    speed.request.value = _maxSpeed * std::abs(msg.axes[Y_AXIS]);
  //    _servoUpdate[Pitch] = true;
  //  }
  //  else if (msg.axes[Y_AXIS] > 0.0f)
  //  {
  //    command.data = M_PI * 0.5f;
  //    speed.request.value = _maxSpeed * std::abs(msg.axes[Y_AXIS]);
  //    _servoUpdate[Pitch] = true;
  //  }
  //  else
  //  {
  //    command.data = _servoStatus[Pitch].angle;
  //    speed.request.value = MIN_SPEED;
  //    _servoUpdate[Pitch] = false;
  //  }
  //
  //  if (speed.request.value < MIN_SPEED)
  //    speed.request.value = MIN_SPEED;
  //
  //  _srvSpeed[Pitch].call(speed);
  //  //    _pubCommand[Pitch].publish(command);
}


void SensorHead::callbackOhmJoy(const ohm_actors_msgs::SensorHeadJoy& msg)
{
  std_msgs::Float32 pos;
  std_msgs::Float32 speed;

  if(msg.home)
  {
    pos.data        = _homing_yaw;
    _servoUpdate[Yaw]   = true;
    _pubCommand[Yaw].publish(pos);
    pos.data = _homing_pitch;
    _servoUpdate[Pitch] = true;
    _pubCommand[Pitch].publish(pos);
  }
  else
  {
    _servoUpdate[Yaw] = true;
    speed.data = _maxSpeed * msg.yaw;
    _pubSpeed[Yaw].publish(speed);
    _servoUpdate[Pitch] = true;
    speed.data = _maxSpeed * msg.pitch;
    _pubSpeed[Pitch].publish(speed);
  }
}


void SensorHead::callbackOculus(const geometry_msgs::PoseStamped& msg)
{
  std_msgs::Float64 command;
  tf::Quaternion quaternion;
  double roll, pitch, yaw;

  tf::quaternionMsgToTF(msg.pose.orientation, quaternion);
  tf::Matrix3x3(quaternion).getRPY(roll, pitch, yaw);

  command.data = pitch;
  _pubCommand[Pitch].publish(command);

  command.data = yaw;
  _pubCommand[Yaw].publish(command);
}

void SensorHead::callbackDirection(const geometry_msgs::QuaternionStamped& msg)
{
  _directionYaw = Eigen::Quaternionf(msg.quaternion.w, msg.quaternion.x, msg.quaternion.y, msg.quaternion.z)
  .matrix().eulerAngles(0, 1, 2)[2];
  _Rmt = Eigen::Quaternionf(msg.quaternion.w, msg.quaternion.x, msg.quaternion.y, msg.quaternion.z).matrix();
}

void SensorHead::callbackLookAtPoint(const geometry_msgs::Point& msg)
{
  //std::cout << __PRETTY_FUNCTION__ << " hallo " << std::endl;
  _lookAtPoint = msg;
}

//void SensorHead::callbackTimerMain(const ros::TimerEvent&)
//{
//  this->spinOnce();
//}

bool SensorHead::callBackSrvSerHoming(ohm_actors_msgs::SetHomingSensHead::Request& req, ohm_actors_msgs::SetHomingSensHead::Response& res)
{
  _homing_pitch = req.homingPitch;
  _homing_yaw = req.homingYaw;

  std::vector<double> vals(100);
  double sum = 0.0;
  for(unsigned int i = 0; i < 10; i++)
    sum += vals[i];


return true;
}

void SensorHead::callBackTimerMain(const ros::TimerEvent& ev)
{
  tf::Transform transform;
   tf::Quaternion rotation;

   tf::Vector3 vec(0.0, 0.0, 0.0);
   transform.setOrigin(vec);
   rotation.setRPY(0.0, 0.0, _angleYaw.data);
   transform.setRotation(rotation);
   _broadcaster.sendTransform(tf::StampedTransform(transform,
       ros::Time::now(),
       _tfServoSource[Yaw],
       _tfServoTarget[Yaw]));

   rotation.setRPY(0.0, -_anglePitch.data, 0.0);
   transform.setRotation(rotation);
   _broadcaster.sendTransform(tf::StampedTransform(transform,
       ros::Time::now(),
       _tfServoSource[Pitch],
       _tfServoTarget[Pitch]));
}

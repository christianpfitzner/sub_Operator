#include "MaxonDifferentialDrive.h"
#include "params.h"

#include <iostream>
using namespace std;

MaxonDifferentialDrive::MaxonDifferentialDrive()
{
  _track               = TRACK;
  _wheelBase           = WHEELBASE;
  _gearRatio           = GEARRATIO;
  _wheelCircumference  = WHEELCICRUMFERENCE;
  _vMax                = _motor.getRPMMax() / _gearRatio * WHEELCICRUMFERENCE;

  _diagonal            = sqrt(_wheelBase*_wheelBase + _track*_track);
  _cosa                = cos(_track / _diagonal);

  ros::NodeHandle prvNh("~");
  prvNh.param<double>("lag_error", _lagTime, 1.0);

  _velSub = _nh.subscribe("vel/teleop", 1, &MaxonDifferentialDrive::velocityCallback, this);
}

void MaxonDifferentialDrive::run()
{
  ros::Rate rate(50);
  _lastCmd = ros::Time::now();

  bool run = true;
  while(run)
  {
    ros::spinOnce();

    ros::Duration dt = ros::Time::now() - _lastCmd;
    bool lag = (dt.toSec() > _lagTime);
    if(lag)
    {
      //ROS_WARN_STREAM("Lag detected ... exiting robot control node");
    }

    run = ros::ok();// && !lag;

    rate.sleep();
  }

  _motor.stop();
}

void MaxonDifferentialDrive::velocityCallback(const geometry_msgs::Twist& cmd)
{
  double vl, vr;

  twistToTrackspeed(&vl, &vr, cmd.linear.x, cmd.angular.z);

  cout << "vl: " << vl << ", vr: " << vr << endl;
  double rpmLeft       = trackspeedToTicksPerTurn(vl) * 60.0;
  double rpmRight     = trackspeedToTicksPerTurn(vr) * 60.0;

  _motor.setRPM(rpmLeft, rpmRight);

  _lastCmd = ros::Time::now();
}

void MaxonDifferentialDrive::twistToTrackspeed(double *vl, double *vr, double v, double omega) const
{
  *vr =  -1 * (v - omega * _diagonal / (2.0 * _cosa));
  *vl =       (v + omega * _diagonal / (2.0 * _cosa));
}

void MaxonDifferentialDrive::trackspeedToTwist(double vl, double vr, double *v, double *omega) const
{
  *v     = (vl - vr) / 2.0;
  *omega = (vr + vl) * _cosa / (2.0 * _diagonal);
}

double MaxonDifferentialDrive::trackspeedToTicksPerTurn(double v) const
{
  return (v / _wheelCircumference) * _gearRatio;
}

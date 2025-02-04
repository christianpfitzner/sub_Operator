/*
 * SchroediMC.h
 *
 *  Created on: Apr 26, 2017
 *      Author: phil
 */

#ifndef OHM_SCHROEDI_MC_SRC_SCHROEDIMC_H_
#define OHM_SCHROEDI_MC_SRC_SCHROEDIMC_H_

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Float64MultiArray.h>
#include "ohm_teleop_msgs/FlipperAngle.h" //toDo: in case we get the position control working we could give angles as well...
#include "ohm_schroedi_mc/VelRps.h"
#include "ohm_schroedi_mc/mc_ctrl.h"
#include "ohm_power_unit/pu.h"
#include "ohm_schroedi_mc/DriveCtrl.h"
#include "ohm_schroedi_mc/mc_ret.h"
#include <std_srvs/SetBool.h>
#include <std_srvs/Empty.h>

struct SimplePose
{
  SimplePose(void):
    x(0.0),
    y(0.0),
    yaw(0.0){}
  float x;
  float y;
  float yaw;
};

class SchroediMC
{
  enum Command
  {
    FLIP_PWR = 0,
    FLIP_PWRLESS
  };
public:
  SchroediMC();
  virtual ~SchroediMC();
  void start(void){this->run();}
private:
  void run(void){ros::spin();}
  void callbackMainTimer(const ros::TimerEvent&);
  void callBackVel(const geometry_msgs::Twist& cmd);
  void callBackFlip(const ohm_teleop_msgs::FlipperAngle& cmd);
  void callBackPu(const ohm_power_unit::pu& pu);
  void callBackReturnRpm(const ohm_schroedi_mc::mc_ret& rpm);
  bool callBackFlipperPowerLess(std_srvs::SetBool::Request& req, std_srvs::SetBool::Response& res);
  bool callBackDriveCtrl(ohm_schroedi_mc::DriveCtrl::Request& req, ohm_schroedi_mc::DriveCtrl::Response& res);
  void calcOdom(const ohm_schroedi_mc::mc_ret& rpm);
  void tracksToTwist(const double vl, const double vr, geometry_msgs::Twist* const twist);
  /**
   * 2D Motion model: computes tracks velocity based on linear and angular velocity
   */
  void twistToTrackspeed(double* vl, double* vr, double v, double omega)const;

  /**
   * 2D Motion model: computes linear and angular velocity based on tracks velocity
   */
  void trackspeedToTwist(const double vl, const double vr, double* const v, double* const omega)const;

  /**
   * Translates the metric speed v into the needed motor RPM
   */
  double trackspeedToTicksPerTurn(double v)const;
  void stopMotors(void);
  void stopFlippers(void);
  int32_t angleToFlipperTicks(const float angle);
  ros::NodeHandle _nh;
  ros::Timer _timerMain;
  ros::Time _timerLagChain;
  ros::Time _timerLagFlipper;
  double _timeOutLag;
  ros::Subscriber _subsVel;
  ros::Subscriber _subsFlipper;
  ros::Subscriber _subsRPS;
  ros::Subscriber _subsPu;
  ros::Publisher _pubVelMotor;
  ros::Publisher _pubOdom;
  ros::ServiceServer _serviceFlipperPowerless;
  ros::ServiceServer _serviceResetDrives;
  bool _flagDriveCtrl;
  ohm_schroedi_mc::mc_ctrl _frameRpsMotor;
  //kinematic parameters
  double _vMax;

  // distance of wheels (axis length)
  double _track;

  // distance of axes
  double _wheelBase;

  // ratio between wheel revolution and motor revolution
  double _gearRatio;

  // circumference of wheels
  double _wheelCircumference;

  // diagonal of robot base (wheel contact)
  double _diagonal;

  // cosine of alpha from kinematic center to front right wheel
  double _cosa;

  int _threshRpsMax;

  double _threshES;

  ohm_power_unit::pu _pu;
  ros::Time _timerLagPu;
  bool _esActive;
  ohm_schroedi_mc::mc_ret _rpmCur;
  bool _rpmReceived;
};

#endif /* OHM_SCHROEDI_MC_SRC_SCHROEDIMC_H_ */

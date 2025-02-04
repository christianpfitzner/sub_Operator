/*
 * EposSkidSteerNode.h
 *
 *  Created on: Mar 23, 2017
 *      Author: phil
 */

#ifndef SRC_EPOSSKIDSTEERNODE_H_
#define SRC_EPOSSKIDSTEERNODE_H_

#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include "CEposVMC.h"

namespace autonohm
{

class EposSkidSteerNode
{
public:
  EposSkidSteerNode();
  virtual ~EposSkidSteerNode();
  void start(void);
private:
  void callBackMainTimer(const ros::TimerEvent& e);
  void callBackTwist(const geometry_msgs::Twist& twist);
  void generateOdom(void);
  double track_to_TicksPerTurn(double v);
  void twist_to_tracks(double *vl, double *vr, double v, double omega);
  ros::NodeHandle _nh;
  ros::Timer _timer;
  ros::Subscriber _subsTwist;
  ros::Publisher  _pubOdom;
  tf::TransformBroadcaster _bc;
  epos::CEposVMC _motors;
  double _wheelGauge;
  double _wheelDiameter;
  double _gearRatio;
};

} /* namespace autonohm */

#endif /* SRC_EPOSSKIDSTEERNODE_H_ */

/*
 * DifferentialDrive.cpp
 *
 *  Created on: 21.07.2015
 *      Author: amndan
 */

#include "DifferentialDrive.h"

DifferentialDrive::DifferentialDrive(Motor * leftMotor, Motor* rightMotor,
		double wheelGauge,double wheelDiameter, double gearRatio, std::string odomTfFrame, std::string odomBaseFrame){

	this->leftMotor  = leftMotor;
	this->rightMotor = rightMotor;
	this->wheelGauge    = wheelGauge;
	this->wheelDiameter = wheelDiameter;
	this->gearRatio     = gearRatio;
	this->odomTfFrame = odomTfFrame;
	this->odomBaseFrame = odomBaseFrame;
	this->maxRPM = std::min(rightMotor->getMaxRPM(), leftMotor->getMaxRPM());

	// init odom
	odom.theta = 0.0;
	odom.x = 0.0;
	odom.y = 0.0;
};

DifferentialDrive::~DifferentialDrive() {
	delete leftMotor;
	delete rightMotor;
};


double DifferentialDrive::track_to_TicksPerTurn(double v){
	return (v / (wheelDiameter * PI)) * gearRatio;
};


void DifferentialDrive::twist_to_tracks(double *vl, double *vr, double v, double omega){
	*vr = -1 * (v + omega * wheelGauge / 2.0); // ToDo: Direction var einführen
	*vl =       v - omega * wheelGauge / 2.0;
};

bool DifferentialDrive::move(const geometry_msgs::Twist::ConstPtr& cmd_vel){

	double vr, vl;
	twist_to_tracks(&vl, &vr, cmd_vel->linear.x, cmd_vel->angular.z);

	double tpt_l = track_to_TicksPerTurn(vl) * 60.0;
	double tpt_r = track_to_TicksPerTurn(vr) * 60.0;

	scaleVelToMaxRPM(tpt_l, tpt_r);

	if ((leftMotor->setRPM(tpt_l) && rightMotor->setRPM(tpt_r))){
		return 1;
	} else return 0;
};

void DifferentialDrive::scaleVelToMaxRPM(double& vel1, double& vel2){

  double absVelMax = std::max(std::abs(vel1), std::abs(vel2));

  if(absVelMax > this->maxRPM)
  {
    double scaleFactor = this->maxRPM / absVelMax;
    vel1 *= scaleFactor;
    vel2 *= scaleFactor;
  }
}

void DifferentialDrive::tracks_to_twist(double vl, double vr, double *v, double *omega)
{
	*v     = (vl + vr) / 2.0;
	*omega = (vr - vl) / wheelGauge;
}

geometry_msgs::TransformStamped DifferentialDrive::getOdomTf(){
	return odom_trans;
}

nav_msgs::Odometry DifferentialDrive::getOdomMsg(){
	return odomMsg;
}

bool DifferentialDrive::stop(){
	if(leftMotor->stop() &&	rightMotor->stop()){
		return 1;
	} else return 0;
}

void DifferentialDrive::updateOdom(){

	// for calculating dt
	static ros::Time last_time = ros::Time::now();
	ros::Time current_time = ros::Time::now();

	double v, omega, vl, vr;
	long int rpm_l, rpm_r;
	int comres;

	rpm_l = leftMotor->getRPM();
	rpm_r = rightMotor->getRPM();

	vl =  rpm_l / (60.0 * gearRatio) * wheelDiameter * PI;
	vr = -rpm_r / (60.0 * gearRatio) * wheelDiameter * PI;

	tracks_to_twist(vl, vr, &v, &omega);

	double dt = (current_time - last_time).toSec();
	double dx = v * cos(odom.theta) * dt;
	double dy = v * sin(odom.theta) * dt;
	double dth = omega * dt;

	odom.x     += dx;
	odom.y     += dy;
	odom.theta += dth;

	geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(odom.theta);
	odom_trans.header.stamp = current_time;
	odom_trans.header.frame_id = odomBaseFrame;
	odom_trans.child_frame_id = odomTfFrame;

	odom_trans.transform.translation.x = odom.x;
	odom_trans.transform.translation.y = odom.y;
	odom_trans.transform.translation.z = 0.0;
	odom_trans.transform.rotation = odom_quat;

	geometry_msgs::PoseWithCovariance p;
	p.pose.position.x    = odom.x;
	p.pose.position.y    = odom.y;
	p.pose.position.z    = 0.0;
	p.pose.orientation   = odom_quat;
	p.covariance[0]      = 1e-8;	// acc x
	p.covariance[7]      = 1e-8;	// acc y
	p.covariance[14]     = 9999;	// acc z
	p.covariance[21]     = 9999;	// roll
	p.covariance[28]     = 9999;	// pitch
	p.covariance[35]     = 1e-2;	// yaw

	odomMsg.header.stamp = current_time;
	odomMsg.header.frame_id = odomBaseFrame;
	odomMsg.pose = p;
	odomMsg.pose.pose.position.x = odom.x;
	odomMsg.pose.pose.position.y = odom.y;
	odomMsg.pose.pose.position.z = 0.0;
	odomMsg.pose.pose.orientation = odom_quat;
	odomMsg.child_frame_id = odomTfFrame;
	odomMsg.twist.twist.linear.x = v;
	odomMsg.twist.twist.linear.y = 0.0;
	odomMsg.twist.twist.angular.z = omega;

	last_time = current_time;
}

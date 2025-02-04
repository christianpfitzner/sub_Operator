/*
 * SummitXlBridgeNode.h
 *
 *  Created on: Jan 16, 2014
 *      Author: phil, Smith
 */

#ifndef SUMMITXLBRIDGENODE_H_
#define SUMMITXLBRIDGENODE_H_

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include "TCP/TCP.h"
#include "SummitSocket_msg/SummitSocket_msg.h"

class SummitXlBridgeNode
{
public:
	SummitXlBridgeNode();
	virtual ~SummitXlBridgeNode();
	void twistStCallBack(const geometry_msgs::Twist& twist);
	void start(void);
private:
	void run(void);

	std::string _serverIP;
	apps::TCP* _socketClient;
	ros::NodeHandle _nh;
	ros::Subscriber _twistStSubs;
	ros::Rate* _rate;
};

#endif /* SUMMITXLBRIDGENODE_H_ */

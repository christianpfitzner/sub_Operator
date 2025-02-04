/*
 * SummitXlBridgeNode.cpp
 *
 *  Created on: Jan 16, 2014
 *      Author: phil, Smith
 */

#include "SummitXlBridgeNode.h"

#include <string>

SummitXlBridgeNode::SummitXlBridgeNode():
_rate(NULL)
{
	std::string strVar;
	ros::NodeHandle prvNh("~");
	double rateVar = 50.0;
	prvNh.param("twist_stamped_topic", strVar, std::string("/simon/vel/teleop"));
	prvNh.param("ip_summit_base", _serverIP, std::string("192.168.1.61"));
	//prvNh.param<double>("loop_rate", rateVar, double(50.0));
	_rate = new ros::Rate(rateVar);

	_twistStSubs = _nh.subscribe(strVar, 2, &SummitXlBridgeNode::twistStCallBack, this);

	_socketClient = new apps::TCP(_serverIP.c_str(), SUMMIT_SOCKET_PORT);
}

SummitXlBridgeNode::~SummitXlBridgeNode()
{
	delete _rate;
	_nh.shutdown();
}

void SummitXlBridgeNode::start(void)
{
    ROS_INFO("try to connect to %s:%d\n",_serverIP.c_str(),SUMMIT_SOCKET_PORT);
	_socketClient->connectOnce();
	std::cout << "connected:" << std::endl;
	this->run();
}

void SummitXlBridgeNode::run(void)
{


	while(ros::ok())
	{
	    ros::spin();
		//ros::spinOnce();
		//_rate->sleep();
	}
}

void SummitXlBridgeNode::twistStCallBack(const geometry_msgs::Twist& twist)
{
	summit::SummitSocket_msg msgSocket;
	msgSocket.linearSpeed  = twist.linear.x;
	msgSocket.angularSpeed = twist.angular.z;
	std::cout << __PRETTY_FUNCTION__ << " lin rec = " << msgSocket.linearSpeed << " ang rec "
			  << msgSocket.angularSpeed << "\n";
	_socketClient->write(&msgSocket, sizeof(msgSocket));
}

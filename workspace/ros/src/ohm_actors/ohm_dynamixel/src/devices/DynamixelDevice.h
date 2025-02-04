/*
 * DynamixelDevice.h
 *
 *  Created on: Mar 13, 2018
 *      Author: phil
 */

#ifndef ROS_SRC_NEW_DYN_SRC_DEVICES_DYNAMIXELDEVICE_H_
#define ROS_SRC_NEW_DYN_SRC_DEVICES_DYNAMIXELDEVICE_H_

#include <cstdint>
#include <string>

#include <std_msgs/Float32.h>
#include <ros/ros.h>

#include <dynamixel_workbench_toolbox/dynamixel_workbench.h>

namespace ohm_dynamixel
{

class DynamixelDevice
{
public:
	struct FlagsPub
	{
		FlagsPub(const bool pubPosIn = false, const bool pubVelIn = false, const bool pubCurrentIn = false):
			pubPos(pubPosIn),
			pubVel(pubVelIn),
			pubCurrent(pubCurrentIn)
		{}
		FlagsPub(const FlagsPub& flags):
			pubPos(flags.pubPos),
			pubVel(flags.pubVel),
			pubCurrent(flags.pubCurrent)
		{}
		bool pubPos;
		bool pubVel;
		bool pubCurrent;
	};
	enum class Mode
	{
		VEL = 0,
	  POS
	};
	DynamixelDevice(const uint8_t& devId, const uint32_t& speedPosMode, const std::string& jointName, ros::NodeHandle& nh, const FlagsPub& flagsPub);
	virtual ~DynamixelDevice();

	const uint8_t id(void)const{return _devId;}
	const float& desPos(void)const{return _desPos;}
	void controlSpeed(const uint32_t curPos, uint32_t* const pos, uint32_t* const speed);
	void callBackSpeed(const std_msgs::Float32& msg);
	void callBackPosition(const std_msgs::Float32& msg);
	const uint32_t& speedPosMode(void)const{return _speedPosMode;}
	const std::string& jointName(void)const{return _jointName;}
	void pubPos(const float& pos);
	void pubData(DynamixelWorkbench& dxlWb);
	void update(DynamixelWorkbench& dxlWb);
	const Mode& mode(void)const{return _mode;}
protected:
	virtual uint32_t    maxAngle(void)const   = 0;
	virtual uint32_t    minAngle(void)const   = 0;
	virtual float       resolution(void)const = 0;
	virtual std::string type(void)const       = 0;
private:
	uint32_t speedRadToTicks(const float speedRad);
	Mode _mode;
	float _desSpeed;
	float _desPos;
	uint8_t _devId;
	uint32_t _hysthVelCtrl;
	bool _hysthReached;
	uint32_t _errorControlling;
	uint32_t _speedPosMode;
	std::string _jointName;
	ros::Publisher _pubPos;
	ros::Publisher _pubVel;
	ros::Publisher _pubCurrent;
	FlagsPub _flagsPub;
	float _angPresent;
	float _spdPresent;
	float _curPresent;
  uint32_t _posLast;
  uint32_t _posStop;
};

} /* namespace ohm_dynamixel */

#endif /* ROS_SRC_NEW_DYN_SRC_DEVICES_DYNAMIXELDEVICE_H_ */

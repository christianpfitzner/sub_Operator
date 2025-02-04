/*
 * DeviceData.h
 *
 *  Created on: Mar 14, 2018
 *      Author: phil
 */

#ifndef ROS_SRC_NEW_DYN_SRC_DEVICES_DEVICEDATA_H_
#define ROS_SRC_NEW_DYN_SRC_DEVICES_DEVICEDATA_H_

#include <string>

#define AX12AMAX 1023
#define AX12AMIN 0
#define AX12ARES 0.29
const std::string AX12ATYPE = "AX-12A";

#define MX28MAX 4095
#define MX28MIN 0
#define MX28RES 0.088
const std::string MX28TYPE = "MX-28";

#define XH430V350MAX 4095
#define XH430V350MIN 0
#define XH430V350RES 0.088
const std::string XH430V350TYPE = "XH430-V350";

#define MX106MAX 4095
#define MX106MIN 0
#define MX106RES 0.088
const std::string MX106TYPE = "MX-106";

#define XH540V270MAX 4095
#define XH540V270MIN 0
#define XH540V270RES 0.088
const std::string XH540V270TYPE = "XH540_V270";

#endif /* ROS_SRC_NEW_DYN_SRC_DEVICES_DEVICEDATA_H_ */

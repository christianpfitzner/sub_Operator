/*
 * SummitSocket_msg.h
 *
 *  Created on: 16.01.2014
 *      Author: m1ch1
 */

#ifndef SUMMITSOCKET_MSG_H_
#define SUMMITSOCKET_MSG_H_

#define SUMMIT_SOCKET_PORT      10125

namespace summit{

struct SummitSocket_msg
{
    double linearSpeed;
    double angularSpeed;
};
}

#endif /* SUMMITSOCKET_MSG_H_ */

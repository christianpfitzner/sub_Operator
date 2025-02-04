/*
 * JoyToVelPlugin.h
 *
 *  Created on: 14.06.2016
 *      Author: chris
 */

#ifndef OHM_TELEOP_OHM_JOY_SRC_PLUGINS_JOYTOVEL_H_
#define OHM_TELEOP_OHM_JOY_SRC_PLUGINS_JOYTOVEL_H_

#include "../plugins/Plugin.h"

namespace ohm_joy {

/**
 * @class   JoyToVel
 * @author  Christian Pfitzner
 */
class JoyToVel : public Plugin
{
public:
   /**
    * Default constructor
    */
   JoyToVel(ros::NodeHandle* _nh);

   virtual ~JoyToVel(void);

   /**
    * Function to publish topic
    */
   virtual void publish(void);
};

} /* namespace ohm_joy */

#endif /* OHM_TELEOP_OHM_JOY_SRC_PLUGINS_JOYTOVEL_H_ */

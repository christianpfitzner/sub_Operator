/*
 * JoyToFlipper.h
 *
 *  Created on: 14.06.2016
 *      Author: chris
 */

#ifndef OHM_TELEOP_OHM_JOY_SRC_PLUGINS_JOYTOFLIPPER_H_
#define OHM_TELEOP_OHM_JOY_SRC_PLUGINS_JOYTOFLIPPER_H_

#include "../plugins/Plugin.h"

namespace ohm_joy {

/**
 * @class   JoyToFlipper
 * @author  Christian Pfitzner
 *
 */
class JoyToFlipper : public Plugin
{
public:
   JoyToFlipper(ros::NodeHandle* _nh);
   /**
    * Default destructor
    */
   virtual ~JoyToFlipper(void);
   /**
    * Function to publish to topic
    */
   virtual void publish(void);
};

} /* namespace ohm_joy */

#endif /* OHM_TELEOP_OHM_JOY_SRC_PLUGINS_JOYTOFLIPPER_H_ */

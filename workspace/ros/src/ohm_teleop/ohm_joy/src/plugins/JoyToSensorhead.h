/*
 * JoyToSensorhead.h
 *
 *  Created on: 14.06.2016
 *      Author: chris
 */

#ifndef OHM_TELEOP_OHM_JOY_SRC_PLUGINS_JOYTOSENSORHEAD_H_
#define OHM_TELEOP_OHM_JOY_SRC_PLUGINS_JOYTOSENSORHEAD_H_


#include "../plugins/Plugin.h"


namespace ohm_joy {

class JoyToSensorhead : public Plugin
{
public:
   /**
    * Default constructor
    */
   JoyToSensorhead(ros::NodeHandle* nh);
   /**
    * Default destructor
    */
   virtual ~JoyToSensorhead(void);
   /**
    * Function to publish
    */
   virtual void publish(void);
};

} /* namespace ohm_joy */

#endif /* OHM_TELEOP_OHM_JOY_SRC_PLUGINS_JOYTOSENSORHEAD_H_ */

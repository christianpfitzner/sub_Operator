/*
 * JoyToManipulator.h
 *
 *  Created on: 14.06.2016
 *      Author: chris
 */

#ifndef OHM_TELEOP_OHM_JOY_SRC_PLUGINS_JOYTOMANIPULATOR_H_
#define OHM_TELEOP_OHM_JOY_SRC_PLUGINS_JOYTOMANIPULATOR_H_


#include "../plugins/Plugin.h"

namespace ohm_joy {

/**
 * @class   JoyToManipulator
 * @author  Christian Pfitzner
 */
class JoyToManipulator : public Plugin
{
public:
   /**
    * Default constructor
    */
   JoyToManipulator(ros::NodeHandle* nh);
   /**
    * Default destructor
    */
   virtual ~JoyToManipulator(void);
   /**
    * Function to publish
    */
   virtual void publish(void);
};

} /* namespace ohm_joy */

#endif /* OHM_TELEOP_OHM_JOY_SRC_PLUGINS_JOYTOMANIPULATOR_H_ */

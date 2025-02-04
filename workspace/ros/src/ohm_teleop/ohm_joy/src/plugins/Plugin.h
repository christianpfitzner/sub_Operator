/*
 * Plugin.h
 *
 *  Created on: 14.06.2016
 *      Author: chris
 */

#ifndef OHM_TELEOP_OHM_JOY_SRC_PLUGINS_PLUGIN_H_
#define OHM_TELEOP_OHM_JOY_SRC_PLUGINS_PLUGIN_H_


#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include "../JoyMapper.h"

namespace ohm_joy {

class Plugin
{
public:
   /**
    * Default constructor
    */
   Plugin(ros::NodeHandle* nh)
   : _nh(nh)
   {

   }
   /**
    * default destructor
    */
   virtual ~Plugin(void){

   }

   void loadConfig(void)
   {
      // implement this
   }

   // SETTER
   /**
    * Function to set remapped joy message
    */
   void setRemappedJoyMsg(sensor_msgs::Joy msg) { _msg = msg; }
   /**
    * Function to set mapper
    */
   void setMapper(JoyMapper* mapper)            { _mapper = mapper; }


   // PROCESSING
   /**
    * Function to publish message
    */
   virtual void publish(void) = 0;

protected:
   ros::NodeHandle*              _nh;      //!< pointer on node handle
   std::vector<ros::Publisher>   _pub;     //!< publisher
   sensor_msgs::Joy              _msg;
   JoyMapper*                    _mapper;
};

} /* namespace ohm_joy */

#endif /* OHM_TELEOP_OHM_JOY_SRC_PLUGINS_PLUGIN_H_ */

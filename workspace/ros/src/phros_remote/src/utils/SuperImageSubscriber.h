/*
 * SuperImageSubscriber.h
 *
 *  Created on: May 8, 2019
 *      Author: phil
 */

#ifndef OHM_RQT_SRC_REMOTE_UTILS_SUPERIMAGESUBSCRIBER_H_
#define OHM_RQT_SRC_REMOTE_UTILS_SUPERIMAGESUBSCRIBER_H_

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <memory>
#include <sensor_msgs/Image.h>

namespace phros_remote
{

class SuperImageSubscriber
{
public:
  SuperImageSubscriber(const std::string& topic, const ros::Duration& timeout);
  virtual ~SuperImageSubscriber();
  void stop(void);
  void start(void);
  void callBack(const sensor_msgs::ImageConstPtr& msg);
  std::shared_ptr<sensor_msgs::Image> data(void)const;
private:
  std::string _topic;
  ros::Duration _timeout;
  ros::NodeHandle _nh;
  image_transport::ImageTransport _it;
  image_transport::Subscriber _subs;
  std::shared_ptr<sensor_msgs::Image> _data;
  ros::Time _lastUpdate;
};

} /* namespace ohm_remote */

#endif /* OHM_RQT_SRC_REMOTE_UTILS_SUPERIMAGESUBSCRIBER_H_ */

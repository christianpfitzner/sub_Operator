/*
 * SuperImageSubscriber.cpp
 *
 *  Created on: May 8, 2019
 *      Author: phil
 */

#include "SuperImageSubscriber.h"

namespace ohm_remote
{

SuperImageSubscriber::SuperImageSubscriber(const std::string& topic, const ros::Duration& timeout):
        _topic(topic),
        _timeout(timeout),
        _it(_nh)
{
  this->start();
}

SuperImageSubscriber::~SuperImageSubscriber()
{
  this->stop();
}

void SuperImageSubscriber::stop(void)
{
  _subs.shutdown();
}

void SuperImageSubscriber::start(void)
{
  _subs = _it.subscribe(_topic, 1, &SuperImageSubscriber::callBack, this, image_transport::TransportHints("compressed"));
}

void SuperImageSubscriber::callBack(const sensor_msgs::ImageConstPtr& msg)
{
  _lastUpdate = ros::Time::now();
  if(!_data)
  {
    _data = std::shared_ptr<sensor_msgs::Image>(new sensor_msgs::Image);
  }
  *_data = *msg;
}

std::shared_ptr<sensor_msgs::Image> SuperImageSubscriber::data(void)const
{
  if(ros::Duration(ros::Time::now() - _lastUpdate) > _timeout)
  {
    return nullptr;
  }
  else
    return _data;
}

} /* namespace ohm_remote */

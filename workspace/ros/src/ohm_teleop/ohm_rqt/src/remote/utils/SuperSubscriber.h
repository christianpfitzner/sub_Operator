/*
 * SuperSubscriber.h
 *
 *  Created on: May 8, 2019
 *      Author: phil
 */

#ifndef OHM_RQT_SRC_REMOTE_UTILS_SUPERSUBSCRIBER_H_
#define OHM_RQT_SRC_REMOTE_UTILS_SUPERSUBSCRIBER_H_

#include <ros/ros.h>

namespace ohm_remote
{

template<typename T>
class SuperSubscriber
{
public:
  SuperSubscriber(const std::string topic, const ros::Duration& timeout);
  virtual ~SuperSubscriber();
  void stop(void);
  void start(void);
  void callBack(const T& msg);
  std::shared_ptr<T> data(void)const;
private:
  ros::Subscriber _subs;
  std::string _topic;
  ros::Duration _timeout;
  std::shared_ptr<T> _data;
  ros::Time _lastUpdate;
};

template<typename T>
SuperSubscriber<T>::SuperSubscriber(const std::string topic, const ros::Duration& timeout):
_topic(topic),
_timeout(timeout)
{
  this->start();
}

template<typename T>
SuperSubscriber<T>::~SuperSubscriber()
{
  this->stop();
}

template<typename T>
void SuperSubscriber<T>::callBack(const T& msg)
{
  _lastUpdate = ros::Time::now();
  if(!_data)
    _data = std::shared_ptr<T>(new T);

    *_data = msg;
}

template<typename T>
void SuperSubscriber<T>::stop(void)
{
  _subs.shutdown();
}

template<typename T>
void SuperSubscriber<T>::start(void)
{
  ros::NodeHandle nh;
  _subs = nh.subscribe(_topic, 1, &SuperSubscriber<T>::callBack, this);
}

template<typename T>
std::shared_ptr<T> SuperSubscriber<T>::data(void)const
{
  if(ros::Duration(ros::Time::now() - _lastUpdate) > _timeout)
  {
    return nullptr;
  }
  else
    return _data;
}

} /* namespace ohm_remote */

#endif /* OHM_RQT_SRC_REMOTE_UTILS_SUPERSUBSCRIBER_H_ */

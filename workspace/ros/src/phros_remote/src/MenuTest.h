#ifndef MENUTEST_H_
#define MENUTEST_H_

#include <QtWidgets/QWidget>
#include <QtCore/QTimer>
#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include "ThreadRos.h"
#include <mutex>
#include "gui/menu/Menu.h"


class MenuTest : public QWidget
{
  Q_OBJECT
  public:
    MenuTest(QWidget* parent = nullptr);
    virtual ~MenuTest(){}
    void paintEvent(QPaintEvent* event)override;
    void callBackJoy(const sensor_msgs::Joy& joy);
      void callBackTimer();
      void callBackImage(const sensor_msgs::ImageConstPtr& image);
    
    public slots:
      void joyCommandReceived(void);
      void imageReceived(void);
      
      signals:
      void newJoyCommand();
      void newImage();
  private:  
    ThreadRos _threadRos;
    ros::NodeHandle _nh;
    QTimer _timer;
    std::mutex _mutexJoy;
    std::mutex _mutexImage;
    sensor_msgs::Joy _joy;
    phros_remote::Menu _menu;
   // image_transport::ImageTransport _it;
    QImage _imageCamera;
};

#endif
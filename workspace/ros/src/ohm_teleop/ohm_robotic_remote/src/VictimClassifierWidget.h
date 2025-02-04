#ifndef __VICTIM_CLASSIFIER_WIDGET_H__
#define __VICTIM_CLASSIFIER_WIDGET_H__

#include <QWidget>
#include <QString>

#include <ros/ros.h>
#include <std_msgs/String.h>

class VictimClassifierWidget : public QWidget
{
public:
    VictimClassifierWidget(QWidget* parent = 0);

    void setNodeHandle(ros::NodeHandle* nh);

protected:
    virtual void paintEvent(QPaintEvent* event);

private:
    void callbackReport(const std_msgs::String& msg);

    ros::NodeHandle* _nh;
    ros::Subscriber _subReport;
    QString _report;
};

#endif

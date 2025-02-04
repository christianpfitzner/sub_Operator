#ifndef __NODE_PLAY_STOP_WIDGET_H__
#define __NODE_PLAY_STOP_WIDGET_H__

#include <QWidget>
#include <QString>

#include <ros/ros.h>

#include <vector>


class Button;

class NodePlayStopWidget : public QWidget
{
    Q_OBJECT

public:
    NodePlayStopWidget(QWidget* parent = 0);
    ~NodePlayStopWidget(void);

    void buildByXmlFile(const QString& file);
    void setNodeHandle(ros::NodeHandle* nh);

private slots:
    void callService(void);

private:
    ros::NodeHandle* _nh;
    std::vector<ros::ServiceClient*> _clients;
    std::vector<Button*> _buttons;
    std::vector<bool> _states;
};

#endif

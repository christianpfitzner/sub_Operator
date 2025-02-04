#ifndef __ROBOT_STATUS_WIDGET__
#define __ROBOT_STATUS_WIDGET__

#include "StarTrekLayout.h"
#include "Button.h"
#include "TextSpinBox.h"

#include <QVector>
#include <QList>
#include <QByteArray>

#include <ohm_perception_msgs/VictimArray.h>
//#include "ohm_arduino_ultrasound_co2_do/RDC_Distance.h"
//#include "ohm_signreader/QrArray.h"
//#include "ohm_signreader/HazArray.h"
//#include "ohm_stateControl/state.h"

#include <ros/ros.h>

class ListWidget;
class ListWidgetModel;
class UltrasoundWidget;

class RobotStatusWidget : public StarTrekLayout
{
    Q_OBJECT

public:
    RobotStatusWidget(QWidget* parent = 0);
    virtual ~RobotStatusWidget(void);

    void setNodeHandle(ros::NodeHandle* nh, const QByteArray& ns = QByteArray());

private slots:
    void changeMainWidget(const int index);
    void changeModel(const int index);
//    void acceptVictimRequest(void);
    void playStopAutonomous(void);
    void markVictimOnMap(void);
    void victimRequest(void);

private:
    void getVictims(void);
//    void callbackQr(const ohm_signreader::QrArray& qrs);
//    void callbackHaz(const ohm_signreader::HazArray& hazs);

    Button* _buttonAccept;
    Button* _buttonReject;
    Button* _buttonAutonomous;
    Button* _autonmousInfo;
    Button* _buttonMarkVictim;
    ListWidget* _list;

    ros::NodeHandle* _nh;
    ros::ServiceClient _srvGetAllVictims;
    ros::ServiceClient _srvGetVictim;
    ros::Subscriber _subHaz;
    ros::Subscriber _subQr;
    QVector<ListWidgetModel*> _models;
    QList<ohm_perception_msgs::Victim> _victimRequests;
    TextSpinBox* _spinBoxVictim;
};

#endif

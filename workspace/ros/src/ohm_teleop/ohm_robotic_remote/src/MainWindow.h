#ifndef __MAIN_WINDOW__
#define __MAIN_WINDOW__

#include "FlatDialog.h"
#include "MapWidget.h"
#include "NodePlayStopWidget.h"
#include "VictimClassifierWidget.h"

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <nav_msgs/OccupancyGrid.h>

#include <QMainWindow>
#include <QList>
#include <QLabel>
#include <QTimer>
#include <QByteArray>


/**
 * @namespace Ui
 */
namespace Ui {
class MainWindow;
}

class GamePadWidget;
class KeyEventBroadcaster;
class QImage;

/**
 * @class   MainWindow
 * @author  Christian Merkl and Christian Pfitzner
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * Default constructor
     */
    MainWindow(void);
    /**
     * Default destructor
     */
    ~MainWindow(void);

protected:
    virtual void keyPressEvent(QKeyEvent* event);

private slots:
   /**
    * Function to set timing
    */
    void tick(void);

    void tickVictimRequest(void);
    /**
     * Slot to change robot namespace
     */
    void changeRobot(void);
    /**
     * Function to switch between to namespaces
     */
    void switchNS(void);


    /**
     * Function to trigger autonomy in rona
     */
    void startAutonomyRona(void);
    /**
     * Functino to set end point to rona
     */
    void sendEndPointToRona(void);

private:
    /**
     * Callback function for driver cam
     * @param image
     */
    void callbackDriverCam(const sensor_msgs::ImageConstPtr& image);
    /**
     * callback function for state
     * @param state
     */
    void callbackState(const std_msgs::String state);
    /**
     * Function to set node handle
     * @param ns
     */
    void setNodeHandle(const QByteArray& ns = QByteArray());

    Ui::MainWindow* _ui;
    NodePlayStopWidget _playStopWidget;

    ros::NodeHandle*            _nh;
    image_transport::Subscriber _subDriverCam;
    QImage*                     _imgDriverCam;
    unsigned char*              _imgDataDriverCam;
    ros::ServiceClient          _cliNamespace;



    ros::Publisher              _trigger_start_pub;
    ros::Publisher              _trigger_end_pub;
    ros::Subscriber             _state_sub;

    FlatDialog                  _victimDialog;

    QLabel*                     _statusLabel;
    QList<unsigned int>         _victimRequests;
    QTimer                      _timer;
    QTimer                      _timerVictim;
    VictimClassifierWidget      _victimClassifier;

    QString                     _namespace;
};

#endif

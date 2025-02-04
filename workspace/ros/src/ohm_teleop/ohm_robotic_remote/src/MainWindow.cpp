#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "GamePadWidget.h"
#include "KeyEventBroadcaster.h"

#include "JoyStickInterface.h"


#include <QDebug>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QImage>
#include <QToolBar>
#include <QLabel>


#include <std_msgs/Bool.h>


#include "SensorTestDialog.h"

#define MSG(x) (qDebug() << __PRETTY_FUNCTION__ << ": " << (x))

MainWindow::MainWindow(void)
    : QMainWindow(),
      _ui(new Ui::MainWindow()),
      _nh(new ros::NodeHandle),
      _imgDriverCam(0),
      _imgDataDriverCam(0),
      _namespace("simon")
{
    _ui->setupUi(this);
    this->setPalette(QPalette(Qt::black));

    QMenuBar* menubar = this->menuBar();
    this->setNodeHandle("/simon/");                                                             // set initial namespace
    QMenu* menu = menubar->addMenu("Robot");
    QAction* action = menu->addAction("Georg");
    action->setData(QVariant("georg"));
    this->connect(action, SIGNAL(triggered()), this, SLOT(changeRobot()));

    action = menu->addAction("Simon");
    action->setData(QVariant("simon"));
    this->connect(action, SIGNAL(triggered()), this, SLOT(changeRobot()));

    menu = menubar->addMenu("Tools");
    menu->addAction("Play Stop", &_playStopWidget, SLOT(show()));
    _playStopWidget.setNodeHandle(_nh);
//    _playStopWidget.buildByXmlFile("/home/chris/workspace/ros/ohm_apps/ohm_robotic_remote/play-stop-config.xml");
    _playStopWidget.buildByXmlFile("play-stop-config.xml");
    _playStopWidget.setMinimumWidth(400);

    /* ROS Stuff */
    this->connect(&_timer,       SIGNAL(timeout()), this, SLOT(tick()));
    this->connect(&_timerVictim, SIGNAL(timeout()), this, SLOT(tickVictimRequest()));


    this->connect(JoyStickInterface::getInstance(), SIGNAL(switchNamespace()), this, SLOT(switchNS()));

       _timer.start(10);
    _timerVictim.start(2000);


    _ui->autonomousNotify->setNodeHandle(_nh, "/georg/");



    // add toolbar
    QToolBar* toolbar = new QToolBar();

    QAction* startupCheck    = toolbar->addAction(QIcon(":/icon/list.png"),   "Startup Check");
    QAction* juryCheck       = toolbar->addAction(QIcon(":/icon/jury.png"),   "Jury Check");
    QAction* shutdownMission = toolbar->addAction(QIcon(":/icon/flag.png"),   "Shutdown Mission");
    toolbar->addSeparator();
    QAction* sendAutontomyStart  = toolbar->addAction("Start Autonomy");  //rona/set_end_point
    QAction* sendEndPoint        = toolbar->addAction("Set End Point");   //rona/set_end_point

    this->addToolBar(toolbar);


    SensorTestDialog*  sensortest = new SensorTestDialog(this, _nh);

//    SensorTestSingletonDialog::getInstance()->setNodeHandle(_nh);
//    SensorTestSingletonDialog::getInstance()->init();

    connect(startupCheck, SIGNAL(triggered(bool)), sensortest, SLOT(exec()));


    connect(sendAutontomyStart, SIGNAL(triggered(bool)), this,       SLOT(startAutonomyRona()));
    connect(sendEndPoint,       SIGNAL(triggered(bool)), this,       SLOT(sendEndPointToRona()));


    _statusLabel= new QLabel("Status Bar Init");
    _statusLabel->setStyleSheet("QLabel { background-color : black; color : yellow; }");
    _ui->statusbar->addWidget(_statusLabel);

//    QTimer::singleShot(1000, this, SLOT(showFullScreen()));
}

MainWindow::~MainWindow(void)
{
   delete _nh;
   delete _ui;
}

void MainWindow::tick(void)
{
   if(!ros::ok()) exit(1);
     ros::spinOnce();
}

void MainWindow::tickVictimRequest(void)
{
   _ui->autonomousNotify->tick();
}


void MainWindow::changeRobot(void)
{
    QAction* sender = qobject_cast<QAction*>(this->sender());

    if (!sender)  return;

    if (sender->data().toString()      == "georg") {
       _namespace = "georg";
        this->setNodeHandle("/georg/");
    }
    else if (sender->data().toString() == "simon") {
        _namespace = "simon";
        this->setNodeHandle("/simon/");
    }
    else
    {
        return;
    }
}

void MainWindow::switchNS(void)
{
   if(_namespace == "simon") {
      this->setNodeHandle("/georg/");
      _namespace = "georg";
   }
   else {
      this->setNodeHandle("/simon/");
      _namespace = "simon";
   }


}

void MainWindow::setNodeHandle(const QByteArray& ns)
{
    QByteArray _ns = ns;

    _ui->_thermoWidget->setNodeHandle(_nh, ns);
    _ui->_statusWidget->setNodeHandle(_nh, ns);
    _victimClassifier.setNodeHandle(  _nh);

    JoyStickInterface::getInstance()->setNodeHandle(_nh);


    /* first shutdown all, because i can't find an overloaded operator=. */
    _subDriverCam.shutdown();

    image_transport::ImageTransport it(*_nh);
    _subDriverCam = it.subscribe(_ns.append("image/driver").data(), 1, &MainWindow::callbackDriverCam, this);

    _state_sub    = _nh->subscribe("path_repeat/state", 1, &MainWindow::callbackState, this);

    _trigger_start_pub = _nh->advertise<std_msgs::Bool>("rona/start_repeat",  1);
    _trigger_end_pub   = _nh->advertise<std_msgs::Bool>("rona/set_end_point", 1);
}




void MainWindow::startAutonomyRona(void)
{
   std_msgs::Bool startRona;
   startRona.data = true;
   _trigger_start_pub.publish(startRona);
}


void MainWindow::sendEndPointToRona(void)
{
   std_msgs::Bool endPointTrigger;
   endPointTrigger.data = true;
   _trigger_end_pub.publish(endPointTrigger);
}


void MainWindow::callbackState(const std_msgs::String state)
{
   _statusLabel->setText(QString::fromStdString(state.data));
}


void MainWindow::keyPressEvent(QKeyEvent* event)
{

}

void MainWindow::callbackDriverCam(const sensor_msgs::ImageConstPtr& image)
{
    cv::Mat mat;
    cv::Mat(image->height, image->width, CV_8UC3, const_cast<unsigned char*>(&image->data[0])).copyTo(mat);

    if (image->encoding == "bgr8")
        cv::cvtColor(mat, mat, CV_BGR2RGB);

    _ui->_rgbWidget->setImage(mat);
}


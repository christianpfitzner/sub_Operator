#include "RobotStatusWidget.h"
#include "UltrasoundWidget.h"
#include "ListWidget.h"
#include "GetPosition.h"

#include <ohm_perception_msgs/GetVictim.h>
#include <ohm_perception_msgs/GetAllVictims.h>

#include <QDebug>

RobotStatusWidget::RobotStatusWidget(QWidget* parent)
    : StarTrekLayout(StarTrekLayout::TopToBottom, parent),
      _list(new ListWidget()),
      _nh(0),
      _models(ListWidgetItem::CountType)
{
   std::cout << __PRETTY_FUNCTION__ << std::endl;

    this->setColor(QColor(0xff, 0x99, 0x66));
    this->setText("Robot Status");
    this->setMainWidget(_list);

    /* setup model spin box */
    for (int i = 0; i < ListWidgetItem::CountType; i++)
        _models[i] = new ListWidgetModel();

    _list->setModel(_models[ListWidgetItem::None]);
    _list->setColor(this->color());

    TextSpinBox* spinBox = new TextSpinBox(this ,this->color());
    spinBox->addItem("Victim");
    this->addSideWidget(spinBox);
    this->connect(spinBox, SIGNAL(indexChanged(const int)), this, SLOT(changeMainWidget(const int)));

    this->addSeparator();

    _spinBoxVictim = new TextSpinBox(this, this->color());
    for (int i = 0; i < ListWidgetItem::CountType; i++)
        _spinBoxVictim->addItem(ListWidgetItem::typeName(static_cast<ListWidgetItem::Type>(i)));

    this->addSideWidget(_spinBoxVictim);
    this->connect(_spinBoxVictim, SIGNAL(indexChanged(const int)), this, SLOT(changeModel(const int)));

    /* setup accept reject button */
    _buttonAccept = new Button;
    _buttonAccept->setMinimumHeight(42);
    _buttonAccept->setText("Accept");
    _buttonAccept->setColor(this->color());
    _buttonAccept->setDisabled(true);
    _buttonAccept->setIcon(QPixmap(":gamepad/button-X-pressed.png"));
    this->addSideWidget(_buttonAccept);
//    this->connect(_buttonAccept, SIGNAL(clicked()), this, SLOT(acceptVictimRequest()));

    _buttonReject = new Button;
    _buttonReject->setMinimumHeight(42);
    _buttonReject->setText("Reject");
    _buttonReject->setColor(this->color());
    _buttonReject->setDisabled(true);
    _buttonReject->setIcon(QPixmap(":gamepad/button-O-pressed.png"));
    this->addSideWidget(_buttonReject);
//    this->connect(_buttonReject, SIGNAL(clicked()), this, SLOT(acceptVictimRequest()));

    /* mark victim on map button */
    _buttonMarkVictim = new Button;
    _buttonMarkVictim->setMinimumHeight(42);
    _buttonMarkVictim->setText("Mark Victim");
    _buttonMarkVictim->setColor(this->color());
    this->addSideWidget(_buttonMarkVictim);
    this->connect(_buttonMarkVictim, SIGNAL(clicked()), this, SLOT(markVictimOnMap()));

    /* autonomous button and indicator */
    this->addSeparator();
//    _buttonAutonomous = new Button;
//    _buttonAutonomous->setMinimumHeight(42);
//    _buttonAutonomous->setText("Autonomous");
//    _buttonAutonomous->setColor(this->color());
//    _buttonAutonomous->setCheckable(true);
//    this->addSideWidget(_buttonAutonomous);
//    this->connect(_buttonAutonomous, SIGNAL(clicked()), this, SLOT(playStopAutonomous()));

//    _autonmousInfo = new Button;
//    _autonmousInfo->setMinimumHeight(42);
//    _autonmousInfo->setColor(this->color());
//    _autonmousInfo->setClickable(false);
//    this->addSideWidget(_autonmousInfo);
}

RobotStatusWidget::~RobotStatusWidget(void)
{
    _list->setModel(0);
    qDeleteAll(_models);
}

void RobotStatusWidget::setNodeHandle(ros::NodeHandle* nh, const QByteArray& ns)
{
    if (!nh)
    {
        qDebug() << "Node Handle is NULL.";
        return;
    }

    _nh = nh;

    /* first shutdown all topics, because there is no operator= implemented. */
//    _subHaz.shutdown();
//    _subQr.shutdown();

    _srvGetAllVictims = _nh->serviceClient<ohm_perception_msgs::GetAllVictims>("/victim/get_all_victims");
    _srvGetVictim = _nh->serviceClient<ohm_perception_msgs::GetVictim>("/victim/get_victim");
//    _subHaz = _nh->subscribe(QByteArray(ns).append("ohm_haz").data(), 10, &RobotStatusWidget::callbackHaz, this);
//    _subQr = _nh->subscribe(QByteArray(ns).append("ohm_qr").data(), 10, &RobotStatusWidget::callbackQr, this);
}

void RobotStatusWidget::getVictims(void)
{
    ohm_perception_msgs::GetAllVictims service;

    if (_srvGetAllVictims.call(service))
    {
        ROS_ERROR_STREAM(__PRETTY_FUNCTION__ << ": can not call service get all victims.");
        return;
    }

    for (std::vector<ohm_perception_msgs::Victim>::const_iterator victim(service.response.victims.victims.begin());
         victim < service.response.victims.victims.end(); ++victim)
    {
        _victimRequests.push_front(*victim);
        _models[ListWidgetItem::VictimRequest]->addItem(new ListWidgetItem(ListWidgetItem::VictimRequest, QString::number(victim->id), "req"));
        _list->update();
    }

//    this->victimRequest();
}
/*
void RobotStatusWidget::callbackQr(const ohm_signreader::QrArray& qrs)
{
    for (std::vector<ohm_signreader::Qr>::const_iterator qr(qrs.qr.begin()); qr < qrs.qr.end(); ++qr)
    {
        _models[ListWidgetItem::None]->addItem(new ListWidgetItem(ListWidgetItem::Qr, QString(qr->message.c_str()), QString()));
        _models[ListWidgetItem::Qr]->addItem(new ListWidgetItem(ListWidgetItem::Qr, QString(qr->message.c_str()), QString()));
        _list->update();
    }
}

void RobotStatusWidget::callbackHaz(const ohm_signreader::HazArray& hazs)
{
    for (std::vector<ohm_signreader::Haz>::const_iterator haz(hazs.haz.begin()); haz < hazs.haz.end(); ++haz)
    {
        _models[ListWidgetItem::None]->addItem(new ListWidgetItem(ListWidgetItem::Hazat, QString(haz->message.c_str()), QString("")));
        _models[ListWidgetItem::Hazat]->addItem(new ListWidgetItem(ListWidgetItem::Hazat, QString(haz->message.c_str()), QString("")));
        _list->update();
    }
}
*/

void RobotStatusWidget::changeMainWidget(const int index)
{
    switch (index)
    {
    case 0:	//victim
        this->setMainWidget(_list);
        _list->setHidden(false);
        _spinBoxVictim->setDisabled(false);
        break;

    default:
        break;
    }
}

void RobotStatusWidget::changeModel(const int index)
{
    if (index < 0)
        return;

    _list->setModel(_models[index]);
    this->victimRequest();
}

void RobotStatusWidget::victimRequest(void)
{
    const bool enable = _models[ListWidgetItem::VictimRequest] == _list->model() && _models[ListWidgetItem::VictimRequest]->items().size();

    _buttonAccept->setEnabled(enable);
    _buttonReject->setEnabled(enable);
    _list->selectRow(0);
}
/*
void RobotStatusWidget::acceptVictimRequest(void)
{
    if (!_models[ListWidgetItem::VictimRequest]->items().size())
        return;

    Button* button = qobject_cast<Button*>(this->sender());
    bool accept = false;

    if (button == _buttonAccept)
        accept = true;
    else if (button == _buttonReject)
        accept = false;
    else
        return;

    ohm_victim::VictimArray victims;
    ohm_victim::Victim victim = _victimRequests.front();
    victim.valid = accept;
    victim.checked = true;
    victims.victims.push_back(victim);
    _pubVictimResponse.publish(victims);

    _victimRequests.pop_front();
    _models[ListWidgetItem::VictimRequest]->removeItem(0);
    _list->update();

    if (!_victimRequests.size())
    {
        _buttonAccept->setEnabled(false);
        _buttonReject->setEnabled(false);
    }
}
*/
void RobotStatusWidget::playStopAutonomous(void)
{

}

void RobotStatusWidget::markVictimOnMap(void)
{
   std::cout << __PRETTY_FUNCTION__ << std::endl;

    GetPosition getPosition(_nh);
    ohm_perception_msgs::Victim victim = getPosition.pos();

    ohm_perception_msgs::VictimArray victims;
    victims.victims.push_back(victim);
//    _pubVictimResponse.publish(victims);
}

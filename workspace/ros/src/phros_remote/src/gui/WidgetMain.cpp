/*
 * WidgetMain.cpp
 *
 *  Created on: May 7, 2019
 *      Author: phil
 */

#include "WidgetMain.h"
#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <ros/ros.h>

namespace phros_remote
{

WidgetMain::WidgetMain(QWidget* parent)
    : QWidget(parent)
    , _actionTriggered(false)
    , _armActive(false)
    , _globalCoordinateSystem(false)
    , _armConstraintsActive(false)
    , _imageMain(std::unique_ptr<QImage>(new QImage))
    , _imageGripper(std::unique_ptr<QImage>(new QImage))
//_iconMode(std::make_unique<QImage>())
{
  //  if(!parent)
  //  {
  //    std::cout << __PRETTY_FUNCTION__ << " huhu" << std::endl;
  //    abort();
  //  }
  ros::NodeHandle prvNh("~");
  prvNh.param<double>("max_48_v", _max48V, 48.9);
  prvNh.param<double>("max_24_v", _max24V, 24.9);
  _pervModel = std::unique_ptr<PerspectiveModel>(new PerspectiveModel(3.0, 3.0, 0.5, 0.2, 0.4, 1.0));
  _iconCs.load(":/menu_icons/gripper.png");
  _iconYouShallNotPass.load(":/menu_icons/youshallnotpass.png");
  //_iconsConst[static_cast<unsigned int>(IconsConst::YOU_SHALL_NOT_PASS)] =  std::shared_ptr<QImage>(new QImage(image));

  // abort();
}

WidgetMain::~WidgetMain()
{
  // TODO Auto-generated destructor stub
}

void WidgetMain::paintEvent(QPaintEvent* event)
{
  //  std::cout << __PRETTY_FUNCTION__ << "call" << std::endl;
  //std::cout << __PRETTY_FUNCTION__ << " flag pad init " << _flagPadIntialized << std::endl;
  QPainter painter(this);
  if(!_imageMain->size().isEmpty())
  {
    painter.drawImage(this->rect(), *_imageMain, _imageMain->rect());
  }
  const double boxEdge  = static_cast<double>(this->rect().width()) / 3500.0 * 3.0;
  const double boxEdgeY = static_cast<double>(this->rect().height()) / 2000.0 * 3.0;

  // std::cout << __PRETTY_FUNCTION__ << " boxedge = " << this->rect().width() << " /  2400 "  << std::endl;
  // std::cout << __PRETTY_FUNCTION__ << " boxedgeY = " << this->rect().height() << " /  600 "  << std::endl;

  // std::cout << __PRETTY_FUNCTION__ << " edge edge y " << boxEdge << " " << boxEdgeY << std::endl;

  _pervModel->resize(boxEdgeY, boxEdge, 0.5, 0.2, 0.4, 1.0);
  int w = this->rect().width() / 8;
  int h = this->rect().height() / 24;

  const int wShift           = static_cast<int>(std::round(static_cast<float>(w) * 1.2));
  const int hShift           = -static_cast<int>(std::round(static_cast<float>(h) * 1.2));
  QPoint    barTopLeftCorner = this->rect().topRight() - QPoint(wShift, hShift);
  QRect     barRect(barTopLeftCorner.x(), barTopLeftCorner.y(), w, h);

  barRect.moveTopLeft(barRect.topLeft() + QPoint(0, h));

  if(_iconMode)
  {
    QRect rectDriveMode(0, 0, this->rect().width() / 12, this->rect().height() / 12);
    rectDriveMode.moveTopLeft(barRect.bottomLeft() + QPoint(this->rect().width() / 20, this->rect().height() / 20));
    painter.drawImage(rectDriveMode, *_iconMode, _iconMode->rect());
  }
  if(_menu)
  {
    QRect menuRect;
    menuRect = QRect(0, 0, this->rect().width() / 4, this->rect().height() / 4);
    menuRect.moveCenter(this->rect().center());
    painter.drawImage(menuRect, _menu->menuIcon(), _menu->menuIcon().rect());
    if(_actionTriggered)
    {
      painter.save();
      QPen pen(Qt::DashLine);
      pen.setColor(Qt::red);
      pen.setWidth(4);
      painter.setPen(pen);
      painter.drawRect(menuRect);
      painter.restore();
    }

    QRect menuRectSMall(0, 0, this->rect().width() / (4 * 2), this->rect().height() / (4 * 2));
    menuRectSMall.moveCenter(this->rect().center() - QPoint(menuRectSMall.width() + 10, 0));
    if(&_menu->previous())
    {
      // std::cout << __PRETTY_FUNCTION__ << "draw previous " << _menu->previous().type() <<std::endl;
      painter.drawImage(menuRectSMall, _menu->previous().menuIcon(), _menu->previous().menuIcon().rect());
    }
    if(&_menu->next())
    {
      // std::cout << __PRETTY_FUNCTION__ << "draw next " << _menu->next().type() <<std::endl;
      menuRectSMall.moveCenter(this->rect().center() + QPoint(menuRectSMall.width() + 10, 0));
      painter.drawImage(menuRectSMall, _menu->next().menuIcon(), _menu->next().menuIcon().rect());
    }

    if(&_menu->up())
    {
      menuRectSMall.moveCenter(this->rect().center() - QPoint(0, menuRectSMall.height() + 10));
      painter.drawImage(menuRectSMall, _menu->up().menuIcon(), _menu->up().menuIcon().rect());
    }

    if(&_menu->down())
    {
      menuRectSMall.moveCenter(this->rect().center() + QPoint(0, menuRectSMall.height() + 10));
      painter.drawImage(menuRectSMall, _menu->down().menuIcon(), _menu->down().menuIcon().rect());
    }
  }
  // draw bars which represent the battery state
  painter.save();
  QString label24V;
  if(!_power)
  {
    label24V = "No Data";
    this->drawEnergyBar(0.0f, _max24V, barRect, painter, BarColors(Qt::green, Qt::yellow, Qt::red), label24V, AlarmLevels(0.5, 0.2));
  }
  else
  {
    QString num;
    num.setNum(_power->r_24V, 'g', 4);
    label24V = num + "V";
    this->drawEnergyBar(_power->r_24V, _max24V, barRect, painter, BarColors(Qt::green, Qt::yellow, Qt::red), label24V, AlarmLevels(0.5, 0.2));
  }

  painter.restore();
  barRect.moveTopLeft(barRect.topLeft() + QPoint(0, h));
  painter.save();
  QString label48VR;

  if(!_power)
  {
    label48VR = "No Data";
    this->drawEnergyBar(0, _max48V, barRect, painter, BarColors(Qt::green, Qt::yellow, Qt::red), label48VR, AlarmLevels(0.5, 0.2));
  }
  else if(_power->r_48V < _max48V / 4.0)
  {
    label48VR = "EM STOP";
    this->drawEnergyBar(_power->r_48V, _max48V, barRect, painter, BarColors(Qt::green, Qt::yellow, Qt::red), label48VR, AlarmLevels(0.5, 0.2));
  }
  else
  {
    QString num;
    num.setNum(_power->r_48V, 'g', 4);
    label48VR = num + "V";
    this->drawEnergyBar(_power->r_48V, _max48V, barRect, painter, BarColors(Qt::green, Qt::yellow, Qt::red), label48VR, AlarmLevels(0.5, 0.2));
  }
  if(_pervModel)
  {
    QPen pen(Qt::SolidLine);
    pen.setWidth(4);
    painter.save();
    painter.translate(50, this->rect().height() - 200);
    painter.setPen(pen);
    std::vector<double> angles(4, 1.0);
    _pervModel->perspectiveTransform();
    _pervModel->paintPerspectiveModel(angles, &painter);
    painter.restore();
  }
  if(_armActive)
  {
    // std::cout << __PRETTY_FUNCTION__ << "arm active" << std::endl;
    if(_globalCoordinateSystem)
    // if(_iconReference)
    {
      QRect rectReference(0, 0, this->rect().width() / 4, this->rect().height() / 4);
      // rectReference.moveTopLeft(barRect.bottomLeft() + QPoint(-50, 100));
      rectReference.moveTopLeft(this->rect().topLeft() + QPoint(this->rect().width() / 10, 0));
      qDebug() << __PRETTY_FUNCTION__ << " draw at" << rectReference;
      painter.drawImage(rectReference, _iconCs, _iconCs.rect());
    }
    if(_armConstraintsActive)
    {
      //        if(!_iconShallNotPass)
      //          return;
      QRect rectShallNotPass(0, 0, this->rect().width() / 9, this->rect().height() / 8);
      rectShallNotPass.moveTopLeft(this->rect().topLeft() + QPoint(rectShallNotPass.width() / 4, rectShallNotPass.height() / 2));
      painter.drawImage(rectShallNotPass, _iconYouShallNotPass, _iconYouShallNotPass.rect());
    }
  }
  if(!_flagPadIntialized)
  {
    //std::cout << __PRETTY_FUNCTION__ << " draw no init icon " << std::endl;
    QRect rectPadNotInit(0, 0, this->rect().width() / 4, this->rect().height() / 4);
    rectPadNotInit.moveCenter(this->rect().center());
    QImage iconPadNotInit;
    iconPadNotInit.load(":/menu_icons/pad_not_initialized.png");
    painter.drawImage(rectPadNotInit, iconPadNotInit, iconPadNotInit.rect());
  }

  painter.restore();
  // std::cout << __PRETTY_FUNCTION__ << " exit" << std::endl;
}

// void WidgetMain::setIconMode(QImage& icon)
// {
//   std::cout << __PRETTY_FUNCTION__ << "call" << std::endl;
//   // if (!_iconMode)
//   //   _iconMode = new QImage(icon);
//   *_iconMode = icon;
//   std::cout << __PRETTY_FUNCTION__ << "exit" << std::endl;
// }

void WidgetMain::drawEnergyBar(const float& energy, const float& energyMax, const QRect& boundingRect, QPainter& painter, const BarColors colors,
                               const QString& label, const AlarmLevels& alarm)
{
  // std::cout << __PRETTY_FUNCTION__ << "call" << std::endl;
  QPen  pen(Qt::SolidLine);
  float fac = 0.0;
  if(!std::isnan(energy))
    fac = energy / energyMax;
  const int     w = static_cast<int>(std::round(fac * static_cast<float>(boundingRect.width())));
  QRect         dynRect(boundingRect.topLeft().x(), boundingRect.topLeft().y(), w, boundingRect.height());
  QBrush        brush(Qt::SolidPattern);
  const QColor* alarmColor = NULL;
  if(fac < alarm.alarm)
    alarmColor = &colors.alarm;
  else if(fac < alarm.warning)
    alarmColor = &colors.warning;
  else
    alarmColor = &colors.ok;
  pen.setColor(*alarmColor);
  brush.setColor(*alarmColor);
  painter.save();
  painter.setBrush(brush);
  painter.setPen(pen);
  painter.drawRect(dynRect);
  painter.restore();

  pen.setColor(colors.ok);
  pen.setWidth(2);
  painter.setPen(pen);
  painter.drawRect(boundingRect); // draw frame

  pen.setColor(Qt::black);
  painter.setPen(pen);
  QFont serifFont("Times", 10, QFont::Bold);
  painter.setFont(serifFont);
  painter.drawText(boundingRect, Qt::AlignCenter, label);
}

} // namespace phros_remote

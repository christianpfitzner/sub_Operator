/*
 * WidgetHud.cpp
 *
 *  Created on: Apr 24, 2018
 *      Author: phil
 */

#ifndef Q_MOC_RUN
#include <ros/ros.h>
#endif
#include "WidgetHud.h"
#include "Hud.h"
#include "PerspectiveModel.h"

#include <QImage>
#include <QPainter>
#include <QDebug>

#include <cmath>

WidgetHud::WidgetHud(QWidget* parent):
_menu(NULL),
_iconMode(NULL),
_imgSensorHead(NULL),
_imgGripper(NULL),
_current24V(NAN),
_current48V(NAN),
_actionTriggered(false),
_initialized(false),
_max48V(0.0),
_max24V(0.0),
_esActive(true),
_iconReference(NULL),
_pervModel(new PerspectiveModel),
_angleGripperHead(0.0),
_drawAngleGripperHead(true),
_armConstraintsActive(false),
_iconShallNotPass(NULL)
{
  ros::NodeHandle prvNh("~");
  prvNh.param<double>("max_48_v", _max48V, 48.9);
  prvNh.param<double>("max_24_v", _max24V, 24.9);

  this->resize(800, 800);
  _initialized = true;

}

WidgetHud::~WidgetHud()
{
  // TODO Auto-generated destructor stub
}

void WidgetHud::paintEvent(QPaintEvent* event)
{
  //  std::cout << __PRETTY_FUNCTION__ << " huhu " << std::endl;
  if(!_initialized)
    return;
  QPainter painter(this);
  if(_imgSensorHead)
  {
    painter.drawImage(this->rect(), *_imgSensorHead, _imgSensorHead->rect());
  }
  //  else
  //    std::cout << __PRETTY_FUNCTION__ << " nonononnonno " << std::endl;
  int wSub = this->rect().width() / 3;
  int hSub = this->rect().height() / 3;
  QPoint botR = this->rect().bottomRight() - QPoint(wSub, hSub);
  QRect rectSub(botR.x(), botR.y(), wSub, hSub);
  if(_imgGripper)
  {
    //    int w = this->rect().width() / 4;
    //    int h = this->rect().height() / 4;
    //    QPoint botR = this->rect().bottomRight() - QPoint(w, h);
    //    QRect rectSub(botR.x(), botR.y(), w, h);
    painter.drawImage(rectSub, *_imgGripper, _imgGripper->rect());
  }
  if(_pervModel)
  {
    QPen pen(Qt::SolidLine);
    pen.setWidth(4);
    painter.save();
    painter.translate(10, this->rect().height() - 150);
    painter.setPen(pen);
    std::vector<double> angles(4, 1.0);
    _pervModel->perspectiveTransform();
    _pervModel->paintPerspectiveModel(angles, &painter);
    painter.restore();
  }
  //    if(_imageThermal)
  //    {
  //      int w = this->rect().width() / _facSub;
  //      int h = this->rect().height() / _facSub;
  //      QPoint botL = this->rect().bottomLeft() - QPoint(-w, h);
  //      QRect rectSub(botL.x(), botL.y(), w, h);
  //      painter.drawImage(rectSub, *_imageThermal, _imageThermal->rect());
  //    }
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
    painter.drawImage(menuRectSMall, _menu->previous()->menuIcon(), _menu->previous()->menuIcon().rect());

    menuRectSMall.moveCenter(this->rect().center() + QPoint(menuRectSMall.width() + 10, 0));
    painter.drawImage(menuRectSMall, _menu->next()->menuIcon(), _menu->next()->menuIcon().rect());
    if(_menu->up())
    {
      menuRectSMall.moveCenter(this->rect().center() - QPoint(0, menuRectSMall.width() + 10));
      painter.drawImage(menuRectSMall, _menu->next()->menuIcon(), _menu->next()->menuIcon().rect());
    }
    if(_menu->down())
    {
      menuRectSMall.moveCenter(this->rect().center() + QPoint(0, menuRectSMall.width() + 10));
      painter.drawImage(menuRectSMall, _menu->next()->menuIcon(), _menu->next()->menuIcon().rect());
    }

  }

  //draw bars which represent the battery state
  painter.save();
  int w = this->rect().width() / 8;
  int h = this->rect().height() / 24;

  const int wShift = static_cast<int>(std::round(static_cast<float>(w) * 1.2));
  const int hShift = -static_cast<int>(std::round(static_cast<float>(h) * 1.2));
  //
  QPoint barTopLeftCorner = this->rect().topRight() - QPoint(wShift, hShift);
  QRect barRect(barTopLeftCorner.x(), barTopLeftCorner.y(), w, h);
  QString label24V;
  if(std::isnan(_current24V))
    label24V = "No Data";
  else
  {
    QString num;
    num.setNum(_current24V, 'g', 4);
    label24V = num + "V";
  }
  this->drawEnergyBar(_current24V, _max24V, barRect, painter, BarColors(Qt::green, Qt::yellow, Qt::red), label24V, AlarmLevels(0.5, 0.2));
  painter.restore();
  barRect.moveTopLeft(barRect.topLeft() + QPoint(0, h));
  painter.save();
  QString label48VR;

  if(std::isnan(_current48V))
    label48VR = "No Data";
  else if(_esActive)
    label48VR = "EM STOP";
  else
  {
    QString num;
    num.setNum(_current48V, 'g', 4);
    label48VR = num + "V";
  }
  this->drawEnergyBar(_current48V, _max48V, barRect, painter, BarColors(Qt::green, Qt::yellow, Qt::red), label48VR, AlarmLevels(0.5, 0.2));
  painter.restore();

  if(_iconMode)
  {
    QRect rectDriveMode(0, 0, this->rect().width() / 12, this->rect().height() / 12);
    rectDriveMode.moveTopLeft(barRect.bottomLeft() + QPoint(0, 20));
    //  if(1)//!_imgSensorHead)          //todo: this depends on the image shown...
    painter.drawImage(rectDriveMode, *_iconMode, _iconMode->rect());
    //    else
    //    {
    //      unsigned int sumRed = 0;
    //      unsigned int sumGreen = 0;
    //      unsigned int sumBlue = 0;
    //      for(unsigned int i = 0; i < rectDriveMode.height(); i++)
    //      {
    //        for(unsigned int j = 0; j < rectDriveMode.width(); j++)
    //        {
    //          QRgb rgb = _imgSensorHead->pixel(j, i);
    //          QColor color(rgb);
    //          sumRed += color.red();
    //          sumGreen += color.green();
    //          sumBlue += color.blue();
    //        }
    //      }
    //      std::cout << __PRETTY_FUNCTION__ << " " << sumRed << " " << sumGreen << " " << sumBlue << std::endl;
    //      QColor sum(sumRed / (rectDriveMode.width() * rectDriveMode.height()), sumGreen / (rectDriveMode.width() * rectDriveMode.height()), sumBlue / (rectDriveMode.width() * rectDriveMode.height()));
    //      qDebug() << __PRETTY_FUNCTION__ << " " << sum;
    //      QColor invSum(255 - sum.red(), 255 - sum.green(), sum.blue());
    //      QImage invSymb = *_iconMode;
    //      for(unsigned int i = 0; i < invSymb.height(); i++)
    //      {
    //        for(unsigned int j = 0;  j < invSymb.width(); j++)
    //        {
    //          QRgb rgb = _imgSensorHead->pixel(j, i);
    //          QColor color(rgb);
    //          if(color.red() + color.green() + color.blue() < 100)
    //            invSymb.setPixel(j, i, invSum.rgb());
    //        }
    //      }
    //    }
  }
  if(_armActive)
  {
    if(_globalCoordinateSystem)
      //if(_iconReference)
    {
      QRect rectReference(0, 0, this->rect().width() / 4, this->rect().height() / 4);
      //rectReference.moveTopLeft(barRect.bottomLeft() + QPoint(-50, 100));
      rectReference.moveTopLeft(this->rect().topLeft() + QPoint(120, 0));
      painter.drawImage(rectReference, *_iconReference, _iconReference->rect());
    }
    if(_armConstraintsActive)
    {
      if(!_iconShallNotPass)
        return;
      QRect rectShallNotPass(0, 0, this->rect().width() / 9, this->rect().height() / 8);
      rectShallNotPass.moveTopLeft(this->rect().topLeft() + QPoint(rectShallNotPass.width() / 4, rectShallNotPass.height() / 2));
      painter.drawImage(rectShallNotPass, *_iconShallNotPass, _iconShallNotPass->rect());
    }
    if(_drawAngleGripperHead)
    {
      const QChar MathSymbolPi(0x03A0);
      painter.save();
      QRect rectAngleScale = this->rect();
      rectAngleScale.setHeight(this->height() / 20);
      rectAngleScale.setWidth(this->width() - 40);
      rectAngleScale.moveCenter(QPoint(this->rect().center().x(), this->rect().height() / 20));
      QPen pen(Qt::SolidLine);
      pen.setWidth(5);
      pen.setColor(Qt::black);
      painter.setPen(pen);
      //painter.drawRect(rectAngleScale);
      const unsigned int nScales = 10;  //when you change this magic number...nothing works anymore...dont TOUCH. todo: Something
      const int resScales = rectAngleScale.width() / nScales;
      const int xOffset = rectAngleScale.x();
      for(unsigned int i = 0; i <= nScales; i++)
      {
        QLine line(QPoint(xOffset + i * resScales , rectAngleScale.topLeft().y()), QPoint(xOffset + i * resScales, rectAngleScale.topLeft().y() + 7));
        painter.drawLine(line);
      }
      QLine line(QPoint(this->rect().topLeft().x(), rectAngleScale.topLeft().y()),
          QPoint(this->rect().topRight().x(), rectAngleScale.topRight().y()));
      //    pen.setWidth(10);
      //    painter.setPen(pen);
      painter.drawLine(line);
      //painter.drawLine
      QFont serifFont("Times", 15 ,QFont::Bold);
      painter.setFont(serifFont);
      painter.drawText(QPoint(rectAngleScale.center().x() - 5,   rectAngleScale.topLeft().y() + this->rect().height() / 30), "0");
      painter.drawText(QPoint(rectAngleScale.topRight().x() - 7, rectAngleScale.topLeft().y() + this->rect().height() / 30), MathSymbolPi);
      painter.drawText(QPoint(rectAngleScale.topLeft().x() - 12, rectAngleScale.topLeft().y() + this->rect().height() / 30), QString("-") + MathSymbolPi);
      pen.setColor(Qt::blue);
      painter.setPen(pen);
      QLine line2(this->rect().topLeft() + QPoint(0 , this->rect().height() / 100), this->rect().topRight() + QPoint(0 , this->rect().height() / 100));
      painter.drawLine(line2);
      QBrush brush(Qt::SolidPattern);
      brush.setColor(Qt::blue);
      painter.setBrush(brush);
      QPolygon triangle;
      const float angle = _angleGripperHead + M_PI;
      // std::cout << __PRETTY_FUNCTION__ << " angle " << angle << std::endl;
      const int pixel =  xOffset +  static_cast<int>(std::round(angle * (static_cast<float>(rectAngleScale.width()) / (2.0 * M_PI))));
      //std::cout << __PRETTY_FUNCTION__ << " pixel " << pixel << std::endl;
      triangle << QPoint(pixel - 20, this->rect().height() / 100) << QPoint(pixel + 20,
          this->rect().height() / 100) << QPoint(pixel, rectAngleScale.topRight().y());
      painter.drawPolygon(triangle);
      painter.restore();
    }
  }
  if(_qrs.size())
  {
    //    float facX = static_cast<float>(this->width()) / static_cast<float>(_imgSensorHead->width());
    //    float facY = static_cast<float>(this->height()) / static_cast<float>(_imgSensorHead->height());
    float facX = 0.0;
    float facY = 0.0;
    if(_armActive)
    {
      facX = static_cast<float>(this->width()) / static_cast<float>(_imgSensorHead->width());
      facY = static_cast<float>(this->height()) / static_cast<float>(_imgSensorHead->height());
    }
    else
    {
      facX = static_cast<float>(rectSub.width()) / static_cast<float>(_imgGripper->width());
      facY = static_cast<float>(rectSub.height()) / static_cast<float>(_imgGripper->height());
    }
    for(auto& iter : _qrs)
    {
      QPen pen(Qt::SolidLine);
      QPointF offset(0.0, 0.0);
      if(!_armActive)
        offset = static_cast<QPointF>(rectSub.topLeft());
      pen.setWidth(5);
      pen.setColor(Qt::blue);
      painter.setPen(pen);
      QPoint center(iter.center);
      QPointF centerF(static_cast<float>(center.x()) * facX, static_cast<float>(center.y()) * facY);
      painter.drawEllipse(offset + centerF, 2, 2);
      QFont serifFont("Times", 15 ,QFont::Bold);
      painter.setFont(serifFont);
      if(iter.corners.size() > 3)
      {
        QPointF topLeft(static_cast<float>(iter.corners[0].x()) * facX, static_cast<float>(iter.corners[0].y()) * facY);
        QPointF botRight(static_cast<float>(iter.corners[2].x()) * facX, static_cast<float>(iter.corners[2].y()) * facY);
        QRectF rectQr(topLeft, botRight);
        rectQr.translate(offset);
        painter.drawRect(rectQr);
        painter.drawText(rectQr.center() + QPointF(rectQr.width(), 0), iter.message.c_str());
      }
      else
        painter.drawText(centerF + QPointF(10.0, 0) + offset, iter.message.c_str());
    }
    //std::cout << __PRETTY_FUNCTION__ << " qrs " << _qrs.qr.size() << std::endl;
  }

  //  std::cout << __PRETTY_FUNCTION__ << " nene " << std::endl;
}

void WidgetHud::setImageSensorHead(const QImage& image)
{
  static bool initial = true;
  if(initial)
  {
    initial = false;
    this->resize(image.width(), image.height());
  }
  _imgSensorHead = &image;
}

void WidgetHud::setPuVals(const double current24V, const double current48V)
{
  _current24V = current24V;
  _current48V = current48V;
}

void WidgetHud::drawEnergyBar(const float& energy, const float& energyMax, const QRect& boundingRect, QPainter& painter, const BarColors colors, const QString& label, const AlarmLevels& alarm)
{
  QPen pen(Qt::SolidLine);
  float fac = 0.0;
  if(!std::isnan(energy))
    fac = energy / energyMax;
  const int w = static_cast<int>(std::round(fac * static_cast<float>(boundingRect.width())));
  QRect dynRect(boundingRect.topLeft().x(), boundingRect.topLeft().y(), w, boundingRect.height());
  QBrush brush(Qt::SolidPattern);
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
  painter.drawRect(boundingRect);   //draw frame

  pen.setColor(Qt::black);
  painter.setPen(pen);
  QFont serifFont("Times", 10, QFont::Bold);
  painter.setFont(serifFont);
  painter.drawText(boundingRect, Qt::AlignCenter, label);
}

void WidgetHud::paintHudtoImage(QImage* image)
{
  //image->height()
  //  std::cout << __PRETTY_FUNCTION__ << " huhu " << std::endl;
  if(!_initialized)
    return;
  QPainter painter(image);
  if(_imgSensorHead)
  {
    painter.drawImage(this->rect(), *_imgSensorHead, _imgSensorHead->rect());
  }
  //  else
  //    std::cout << __PRETTY_FUNCTION__ << " nonononnonno " << std::endl;
  if(_imgGripper)
  {
    int w = this->rect().width() / 4;
    int h = this->rect().height() / 4;
    QPoint botR = this->rect().bottomRight() - QPoint(w, h);
    QRect rectSub(botR.x(), botR.y(), w, h);
    painter.drawImage(rectSub, *_imgGripper, _imgGripper->rect());
  }
  if(_pervModel)
  {
    QPen pen(Qt::SolidLine);
    pen.setWidth(4);
    painter.save();
    painter.translate(10, this->rect().height() - 150);
    painter.setPen(pen);
    std::vector<double> angles(4, 1.0);
    _pervModel->perspectiveTransform();
    _pervModel->paintPerspectiveModel(angles, &painter);
    painter.restore();
  }
  //    if(_imageThermal)
  //    {
  //      int w = this->rect().width() / _facSub;
  //      int h = this->rect().height() / _facSub;
  //      QPoint botL = this->rect().bottomLeft() - QPoint(-w, h);
  //      QRect rectSub(botL.x(), botL.y(), w, h);
  //      painter.drawImage(rectSub, *_imageThermal, _imageThermal->rect());
  //    }
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
    painter.drawImage(menuRectSMall, _menu->previous()->menuIcon(), _menu->previous()->menuIcon().rect());

    menuRectSMall.moveCenter(this->rect().center() + QPoint(menuRectSMall.width() + 10, 0));
    painter.drawImage(menuRectSMall, _menu->next()->menuIcon(), _menu->next()->menuIcon().rect());
    if(_menu->up())
    {
      menuRectSMall.moveCenter(this->rect().center() - QPoint(0, menuRectSMall.width() + 10));
      painter.drawImage(menuRectSMall, _menu->next()->menuIcon(), _menu->next()->menuIcon().rect());
    }
    if(_menu->down())
    {
      menuRectSMall.moveCenter(this->rect().center() + QPoint(0, menuRectSMall.width() + 10));
      painter.drawImage(menuRectSMall, _menu->next()->menuIcon(), _menu->next()->menuIcon().rect());
    }

  }

  //draw bars which represent the battery state
  painter.save();
  int w = this->rect().width() / 8;
  int h = this->rect().height() / 24;

  const int wShift = static_cast<int>(std::round(static_cast<float>(w) * 1.2));
  const int hShift = -static_cast<int>(std::round(static_cast<float>(h) * 1.2));
  //
  QPoint barTopLeftCorner = this->rect().topRight() - QPoint(wShift, hShift);
  QRect barRect(barTopLeftCorner.x(), barTopLeftCorner.y(), w, h);
  QString label24V;
  if(std::isnan(_current24V))
    label24V = "No Data";
  else
  {
    QString num;
    num.setNum(_current24V, 'g', 4);
    label24V = num + "V";
  }
  this->drawEnergyBar(_current24V, _max24V, barRect, painter, BarColors(Qt::green, Qt::yellow, Qt::red), label24V, AlarmLevels(0.5, 0.2));
  painter.restore();
  barRect.moveTopLeft(barRect.topLeft() + QPoint(0, h));
  painter.save();
  QString label48VR;

  if(std::isnan(_current48V))
    label48VR = "No Data";
  else if(_esActive)
    label48VR = "EM STOP";
  else
  {
    QString num;
    num.setNum(_current48V, 'g', 4);
    label48VR = num + "V";
  }
  this->drawEnergyBar(_current48V, _max48V, barRect, painter, BarColors(Qt::green, Qt::yellow, Qt::red), label48VR, AlarmLevels(0.5, 0.2));
  painter.restore();

  if(_iconMode)
  {
    QRect rectDriveMode(0, 0, this->rect().width() / 12, this->rect().height() / 12);
    rectDriveMode.moveTopLeft(barRect.bottomLeft() + QPoint(0, 20));
    //  if(1)//!_imgSensorHead)          //todo: this depends on the image shown...
    painter.drawImage(rectDriveMode, *_iconMode, _iconMode->rect());
    //    else
    //    {
    //      unsigned int sumRed = 0;
    //      unsigned int sumGreen = 0;
    //      unsigned int sumBlue = 0;
    //      for(unsigned int i = 0; i < rectDriveMode.height(); i++)
    //      {
    //        for(unsigned int j = 0; j < rectDriveMode.width(); j++)
    //        {
    //          QRgb rgb = _imgSensorHead->pixel(j, i);
    //          QColor color(rgb);
    //          sumRed += color.red();
    //          sumGreen += color.green();
    //          sumBlue += color.blue();
    //        }
    //      }
    //      std::cout << __PRETTY_FUNCTION__ << " " << sumRed << " " << sumGreen << " " << sumBlue << std::endl;
    //      QColor sum(sumRed / (rectDriveMode.width() * rectDriveMode.height()), sumGreen / (rectDriveMode.width() * rectDriveMode.height()), sumBlue / (rectDriveMode.width() * rectDriveMode.height()));
    //      qDebug() << __PRETTY_FUNCTION__ << " " << sum;
    //      QColor invSum(255 - sum.red(), 255 - sum.green(), sum.blue());
    //      QImage invSymb = *_iconMode;
    //      for(unsigned int i = 0; i < invSymb.height(); i++)
    //      {
    //        for(unsigned int j = 0;  j < invSymb.width(); j++)
    //        {
    //          QRgb rgb = _imgSensorHead->pixel(j, i);
    //          QColor color(rgb);
    //          if(color.red() + color.green() + color.blue() < 100)
    //            invSymb.setPixel(j, i, invSum.rgb());
    //        }
    //      }
    //    }
  }

  if(_iconReference)
  {
    QRect rectReference(0, 0, this->rect().width() / 16, this->rect().height() / 16);
    rectReference.moveTopLeft(barRect.bottomLeft() + QPoint(15, 100));
    painter.drawImage(rectReference, *_iconReference, _iconReference->rect());
  }
  if(_drawAngleGripperHead)
  {
    const QChar MathSymbolPi(0x03A0);
    painter.save();
    QRect rectAngleScale = this->rect();
    rectAngleScale.setHeight(this->height() / 20);
    rectAngleScale.setWidth(this->width() - 40);
    rectAngleScale.moveCenter(QPoint(this->rect().center().x(), this->rect().height() / 20));
    QPen pen(Qt::SolidLine);
    pen.setWidth(5);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    //painter.drawRect(rectAngleScale);
    const unsigned int nScales = 10;  //when you change this magic number...nothing works anymore...dont TOUCH. todo: Something
    const int resScales = rectAngleScale.width() / nScales;
    const int xOffset = rectAngleScale.x();
    for(unsigned int i = 0; i <= nScales; i++)
    {
      QLine line(QPoint(xOffset + i * resScales , rectAngleScale.topLeft().y()), QPoint(xOffset + i * resScales, rectAngleScale.topLeft().y() + 7));
      painter.drawLine(line);
    }
    QLine line(QPoint(this->rect().topLeft().x(), rectAngleScale.topLeft().y()),
        QPoint(this->rect().topRight().x(), rectAngleScale.topRight().y()));
    //    pen.setWidth(10);
    //    painter.setPen(pen);
    painter.drawLine(line);
    //painter.drawLine
    QFont serifFont("Times", 15 ,QFont::Bold);
    painter.setFont(serifFont);
    painter.drawText(QPoint(rectAngleScale.center().x() - 5,   rectAngleScale.topLeft().y() + this->rect().height() / 30), "0");
    painter.drawText(QPoint(rectAngleScale.topRight().x() - 7, rectAngleScale.topLeft().y() + this->rect().height() / 30), MathSymbolPi);
    painter.drawText(QPoint(rectAngleScale.topLeft().x() - 12, rectAngleScale.topLeft().y() + this->rect().height() / 30), QString("-") + MathSymbolPi);
    pen.setColor(Qt::blue);
    painter.setPen(pen);
    QLine line2(this->rect().topLeft() + QPoint(0 , this->rect().height() / 100), this->rect().topRight() + QPoint(0 , this->rect().height() / 100));
    painter.drawLine(line2);
    QBrush brush(Qt::SolidPattern);
    brush.setColor(Qt::blue);
    painter.setBrush(brush);
    QPolygon triangle;
    const float angle = _angleGripperHead + M_PI;
    // std::cout << __PRETTY_FUNCTION__ << " angle " << angle << std::endl;
    const int pixel =  xOffset +  static_cast<int>(std::round(angle * (static_cast<float>(rectAngleScale.width()) / (2.0 * M_PI))));
    //std::cout << __PRETTY_FUNCTION__ << " pixel " << pixel << std::endl;
    triangle << QPoint(pixel - 20, this->rect().height() / 100) << QPoint(pixel + 20,
        this->rect().height() / 100) << QPoint(pixel, rectAngleScale.topRight().y());
    painter.drawPolygon(triangle);
    painter.restore();
  }
  if(_qrs.size())
  {
    //    float facX = 0.0;
    //    float facY = 0.0;
    //    if(_armActive)
    //    {
    float facX = static_cast<float>(this->width()) / static_cast<float>(_imgSensorHead->width());
    float facY = static_cast<float>(this->height()) / static_cast<float>(_imgSensorHead->height());
    //    }
    //    else
    //    {
    //      facX = static_cast<float>(r  width()) / static_cast<float>(_imgSensorHead->width());
    //      facY = static_cast<float>(this->height()) / static_cast<float>(_imgSensorHead->height());
    //    }
    for(auto& iter : _qrs)
    {
      QPen pen(Qt::SolidLine);
      pen.setWidth(5);
      pen.setColor(Qt::blue);
      painter.setPen(pen);

      QPoint center(iter.center);
      //qDebug() << __PRETTY_FUNCTION__ << " center " << center;
      QPointF centerF(static_cast<float>(center.x()) * facX, static_cast<float>(center.y()) * facY);
      //      qDebug() << __PRETTY_FUNCTION__ << " facX " << facX << " " << facY;
      //      qDebug() << __PRETTY_FUNCTION__ << " centerF " << centerF;
      painter.drawEllipse(centerF, 2, 2);
      QFont serifFont("Times", 15 ,QFont::Bold);
      painter.setFont(serifFont);
      painter.drawText(centerF + QPointF(10.0, 0), iter.message.c_str());
    }
    //std::cout << __PRETTY_FUNCTION__ << " qrs " << _qrs.qr.size() << std::endl;
  }
}

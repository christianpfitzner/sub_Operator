/*
 * WidgetHorizon2.cpp
 *
 *  Created on: Feb 6, 2019
 *      Author: rescuegui
 */

#include "WidgetHorizon2.h"
#include <QtGui/QPainter>
#include <QtGui/QPicture>
#include <QtCore/QDir>
#include <QtWidgets/QApplication>
#include <cmath>
#include <iostream>

WidgetHorizon2::WidgetHorizon2(void)
{
  pitch = 0;
  roll = 0;
}
WidgetHorizon2::~WidgetHorizon2(void)
{

}

void WidgetHorizon2::paintEvent(QPaintEvent* event)
{
  this->setFixedHeight(this->width());      // adapt window height to width, so ellipse appears as circle
  WidgetHorizon2::paintHorizon(pitch,roll); // paint the virtual horizon (blue-green background)
  WidgetHorizon2::paintMask();               // paint mask (circle and lines)
  WidgetHorizon2::paintCursor();             // paint a graphical cursor
  WidgetHorizon2::paintScale();              // paint scale for the mask
  //WidgetHorizon::paintCross(pitch, roll);  // paint floating red cross
  WidgetHorizon2::paintFrame();              // paint a graphical frame
}

void WidgetHorizon2::paintHorizon(float pitch, float roll)
{
  QPainter painter(this);
  QPen pen(Qt::SolidLine);
  QPolygon poly1, poly2;
  QColor blue(150, 200, 255);
  QColor green(0, 180, 0);

  pen.setWidth(1);

  float size = this->width()/2;         // calculate radius of the circle (needed to display the polygons)
  float sin_roll = size*sin(2*roll);  // needed to display the horizon in the correct angle

  // define the polygon that represents the sky
  poly1 << QPoint(0,0)
        << QPoint(this->width(),0)
        << QPoint(this->width(), (PI_2-pitch)/PI*this->height() - sin_roll)
        << QPoint(0, (PI_2-pitch)/PI*this->height() + sin_roll );

  // draw the polygon poly1 (sky)
  pen.setColor(blue);
  painter.setPen(pen);
  painter.setBrush(blue);
  painter.drawConvexPolygon(poly1);

  // define the polygon that represents the ground
  poly2 << QPoint(0, this->height())
        << QPoint(this->width(), this->height())
        << QPoint(this->width(), (PI_2-pitch)/PI*this->height() - sin_roll)
        << QPoint(0, (PI_2-pitch)/PI*this->height() + sin_roll );

  // draw the polygon poly2 (ground)
  pen.setColor(green);
  painter.setPen(pen);
  painter.setBrush(green);
  painter.drawConvexPolygon(poly2);
}


void WidgetHorizon2::paintMask(void)
{
  QPainter painter(this);
  QPen pen(Qt::SolidLine);
  float width = this->width();


  pen.setWidth(3);
  painter.setPen(pen);

  // draw circle:
  //painter.drawEllipse(this->rect());

  // draw lines:
  //      drawLine(x1,  y1   , x2   ,    y2  )
  painter.drawLine(0, width/2, width, width/2);
  pen.setWidth(1);
  painter.setPen(pen);
  painter.drawLine(width/2-0.33*width,  width*0.125, width/2+0.33*width,  width*0.125);
  painter.drawLine(width/2-0.433*width, width*0.25,  width/2+0.433*width, width*0.25);
  painter.drawLine(width/2-0.484*width, width*0.375, width/2+0.484*width, width*0.375);
  painter.drawLine(width/2-0.484*width, width*0.625, width/2+0.484*width, width*0.625);
  painter.drawLine(width/2-0.433*width, width*0.75,  width/2+0.433*width, width*0.75);
  painter.drawLine(width/2-0.33*width,  width*0.875, width/2+0.33*width,  width*0.875);
  // factors for calculating x1 and x2 have been calculated beforehand to minimize computing resources
}


void WidgetHorizon2::paintCross(float pitch, float roll)
{
  float size = this->width()/25; // determines size of the cross

  QColor red(150, 0, 0);
  QPainter painter(this);
  QPen pen(Qt::SolidLine);

  pen.setWidth(2);
  pen.setColor(red);
  painter.setPen(pen);

  // paint horizontal line of the cross:
  painter.drawLine(this->width()/2 - size, (PI_2-pitch)/PI*this->height(),
                   this->width()/2 + size, (PI_2-pitch)/PI*this->height());

  // paint vertical line of the cross:
  painter.drawLine(this->width()/2, (PI_2-pitch)/PI*this->height() + size,
                   this->width()/2, (PI_2-pitch)/PI*this->height() - size);
}


void WidgetHorizon2::paintScale(void)
{
  float width = this->width();
  QPainter painter(this);
  painter.drawText(width*0.55, width*0.125-1, "+68°"); //rounded from originally 67.5°
  painter.drawText(width*0.55, width*0.25 -1, "+45° ");
  painter.drawText(width*0.55, width*0.375-1, "+23°"); //rounded from originally 22.5°
  painter.drawText(width*0.55, width*0.5  -1, " 0°");
  painter.drawText(width*0.55, width*0.625-1, "-23°"); //rounded from originally 22.5°
  painter.drawText(width*0.55, width*0.75 -1, "-45°");
  painter.drawText(width*0.55, width*0.875-1, "-68°"); //rounded from originally 67.5°
}

void WidgetHorizon2::paintFrame(void)
{
  QPainter painter(this);
  QImage img("/home/user/workspace/ros/src/ohm_teleop/ohm_rqt/src/horizon2/image/horizon_frame.png"); // adapt path to your filesystem!
  Q_ASSERT(!img.isNull());
  painter.drawImage(this->rect(), img);
}

void WidgetHorizon2::paintCursor(void)
{
  QRectF target_cursor((this->width()/2)-((this->width()/SCALE)/2), (PI_2-pitch)/PI*this->height(), this->width()/SCALE, this->height()/SCALE );
  QPainter painter(this);
  QImage img("/home/user/workspace/ros/src/ohm_teleop/ohm_rqt/src/horizon2/image/horizon_cursor.png"); // adapt path to your filesystem!
  Q_ASSERT(!img.isNull());
  painter.drawImage(target_cursor, img);
}

void WidgetHorizon2::setPitch(float p)
{
  // check if the angle a has a value that can be displayed
  // if not: set pitch to the maximal allowed value
  if(p > PI_2)
    p = PI_2;
  if(p < -PI_2)
    p = -PI_2;
  pitch = p;
}
void WidgetHorizon2::setRoll(float r)
{
  // check if the angle t has a value that can be displayed
  // if not: set roll to the maximal allowed value
  if(r > PI_4)
    r = PI_4;
  if(r < -PI_4)
    r = -PI_4;
  roll = r;
}




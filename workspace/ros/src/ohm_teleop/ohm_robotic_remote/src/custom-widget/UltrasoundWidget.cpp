#include "UltrasoundWidget.h"
#include "DistanceWidget.h"

#include <QPainter>
#include <QPainter>
#include <QSize>
#include <QDebug>

namespace {
enum Sensors {
    FrontLeft = 0,  //B0
    FrontMid,       //B1
    FrontRight,     //B2
    SideLeft,       //B3
    SideRight,      //B4
    BackLeft,       //B5
    BackMid,        //B6
    BackRight,      //B7
    SensorCount
};

const float MAX_DISTANCE = 0.2;
}

UltrasoundWidget::UltrasoundWidget(QWidget* parent)
    : QWidget(parent),
      _objectImage(":/ultrasound/enterprise.png"),
      _distances(SensorCount, 0)
{
    this->setPalette(QPalette(Qt::black));

    for (unsigned int i = 0; i < SensorCount; i++)
    {
        _distances[i] = new DistanceWidget(this);
        _distances[i]->setColor(QColor(0x99, 0x99, 0xff));
    }

    _distances[FrontLeft]->setAngle(-30.0);
    _distances[FrontMid]->setAngle(0.0);
    _distances[FrontRight]->setAngle(30.0);
    _distances[SideRight]->setAngle(90.0);
    _distances[SideLeft]->setAngle(-90.0);
    _distances[BackRight]->setAngle(150.0);
    _distances[BackLeft]->setAngle(-150.0);
    _distances[BackMid]->setAngle(180.0);
}

void UltrasoundWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawPixmap(_imagePos, _objectImage, _objectImage.rect());
}

void UltrasoundWidget::resizeEvent(QResizeEvent*)
{
    const QSize distanceSize(this->height() / 5 * 2, this->height() / 5 * 2);

    QVector<QPoint> centers(SensorCount);
    centers[FrontMid]   = QPoint(this->width() >> 1, this->height() / 5);
    centers[FrontLeft]  = QPoint(this->width() / 3, this->height() / 4);
    centers[FrontRight] = QPoint(this->width() - centers[FrontLeft].x(), centers[FrontLeft].y());
    centers[SideLeft]   = QPoint(centers[FrontLeft].x(), this->height() >> 1);
    centers[SideRight]  = QPoint(centers[FrontRight].x(), centers[SideLeft].y());
    centers[BackMid]    = QPoint(this->width() >> 1, this->height() - centers[FrontMid].y());
    centers[BackLeft]   = QPoint(centers[FrontLeft].x(), this->height() - centers[FrontLeft].y());
    centers[BackRight]  = QPoint(centers[FrontRight].x(), centers[BackLeft].y());
    _imagePos = QRect(QPoint(centers[FrontLeft].x(), centers[FrontMid].y()), QPoint(centers[BackRight].x(), centers[BackMid].y()));

    for (int i = 0; i < _distances.size(); i++)
    {
        QRect geometry(QPoint(0, 0), distanceSize);

        geometry.moveCenter(centers[i]);
        _distances[i]->setGeometry(geometry);
    }
}

void UltrasoundWidget::sensorValues(const QVector<float>& values)
{
    const int count = values.size() > _distances.size() ? _distances.size() : values.size();

    for (int i = 0; i < count; i++)
    {
        float norm = values[i] / MAX_DISTANCE;

        if (norm > 1.0)
            norm = 1.0;

        _distances[i]->setDistance(norm);
    }
}

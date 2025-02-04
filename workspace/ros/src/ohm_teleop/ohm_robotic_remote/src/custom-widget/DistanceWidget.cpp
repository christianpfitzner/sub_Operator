#include "DistanceWidget.h"

#include <QPoint>
#include <QPainter>
#include <QDebug>

#include <cmath>

namespace {
const float PI = 3.14159265;
const int BAR_COUNT = 5;

const float ANGLE_OUTLINE = 20.0 * PI / 180.0;
const float ANGLE_INLINE  = 15.0 * PI / 180.0;
}

DistanceWidget::DistanceWidget(QWidget* parent)
    : QWidget(parent),
      _minDistance(0.0),
      _maxDistance(1.0),
      _distance(0.0),
      _angle(0.0),
      _color(Qt::blue)
{

}

void DistanceWidget::setAngle(const float angle)
{
    if (angle > 180.0 || angle < -180.0)
        return;

    _angle = angle * PI / 180.0;
    this->resizeEvent(0);
    this->update();
}

void DistanceWidget::setMaxDistance(const float max)
{
    if (max < _minDistance)
        return;

    _maxDistance = max;
    this->update();
}

void DistanceWidget::setMinDistance(const float min)
{
    if (min > _maxDistance)
        return;

    _minDistance = min;
    this->update();
}

void DistanceWidget::setDistance(const float distance)
{
    if (distance > _maxDistance || distance < _minDistance)
        return;

    _distance = distance;
    this->update();
}

void DistanceWidget::setColor(const QColor& color)
{
    _color = color;
    this->update();
}

void DistanceWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QBrush brush(_color);
    QPen pen(_color);
    pen.setWidth(1);
    painter.setBrush(brush);
    painter.setPen(pen);

    const float distanceStep = (_maxDistance - _minDistance) / static_cast<float>(BAR_COUNT);
    float curDistance = distanceStep;

    for (QVector<QPainterPath>::iterator path(_paths.begin()); path < _paths.end(); ++path, curDistance += distanceStep)
    {
        if (curDistance > _distance)
        {
            painter.setBrush(QBrush(_color.lighter(200)));
            painter.setPen(QPen(_color.lighter(200)));
        }

        painter.drawPath(*path);
    }
}

void DistanceWidget::resizeEvent(QResizeEvent*)
{
    QVector<QVector<QPoint> > points(BAR_COUNT);
    const float height = static_cast<float>(this->height() < this->width() ? this->height() : this->width());
    /* I know magic numbers !!! */
    const float BASE_DISTANCE = height * 0.15454545;
    const float BAR_HEIGHT    = height * 0.03636363;
    const float BAR_SPACE     = BAR_HEIGHT;

    /* calc points of the bars */
    float curDistance = BASE_DISTANCE;

    for (int i = 0; i < points.size(); i++, curDistance += BAR_HEIGHT + BAR_SPACE)
    {
        points[i].push_back(QPoint(static_cast<int>(::sin(ANGLE_INLINE)  *   curDistance),
                                   static_cast<int>(::cos(ANGLE_INLINE)  *  -curDistance)));
        points[i].push_back(QPoint(static_cast<int>(::sin(ANGLE_OUTLINE) *   curDistance),
                                   static_cast<int>(::cos(ANGLE_OUTLINE) *  -curDistance)));
        points[i].push_back(QPoint(static_cast<int>(::sin(ANGLE_OUTLINE) *  (curDistance + BAR_HEIGHT)),
                                   static_cast<int>(::cos(ANGLE_OUTLINE) * -(curDistance + BAR_HEIGHT))));
        points[i].push_back(QPoint(static_cast<int>(::sin(ANGLE_INLINE)  *  (curDistance + BAR_HEIGHT)),
                                   static_cast<int>(::cos(ANGLE_INLINE)  * -(curDistance + BAR_HEIGHT))));

        points[i].push_back(QPoint(points[i][3].x() * -1, points[i][3].y()));
        points[i].push_back(QPoint(points[i][2].x() * -1, points[i][2].y()));
        points[i].push_back(QPoint(points[i][1].x() * -1, points[i][1].y()));
        points[i].push_back(QPoint(points[i][0].x() * -1, points[i][0].y()));
    }


    /* transformat points to center and compute paths for the bars */
    const int t_x = (this->width() >> 1);
    const int t_y = (this->height() >> 1);
    const float cos_a = ::cos(_angle);
    const float sin_a = ::sin(_angle);

    _paths.clear();

    for (QVector<QVector<QPoint> >::iterator bar(points.begin()); bar < points.end(); ++bar)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);

        for (QVector<QPoint>::iterator point(bar->begin()); point < bar->end(); ++point)
        {
            QPoint point_c(static_cast<int>(static_cast<float>(point->x()) * cos_a - static_cast<float>(point->y()) * sin_a) + t_x,
                           static_cast<int>(static_cast<float>(point->x()) * sin_a + static_cast<float>(point->y()) * cos_a) + t_y);

            if (point == bar->begin())
                path.moveTo(point_c);
            else
                path.lineTo(point_c);
        }

        if (!path.isEmpty())
            path.lineTo(path.elementAt(0).x, path.elementAt(0).y);

        _paths.push_back(path);
    }
}

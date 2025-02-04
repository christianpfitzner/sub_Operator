#ifndef __DISTANCE_WIDGET__
#define __DISTANCE_WIDGET__

#include <QWidget>
#include <QVector>
#include <QPainterPath>
#include <QColor>

class DistanceWidget : public QWidget
{
    Q_OBJECT

public:
    DistanceWidget(QWidget* parent = 0);

    void setAngle(const float angle);
    void setMaxDistance(const float max);
    void setMinDistance(const float min);
    void setColor(const QColor& color);

public slots:
    void setDistance(const float distance);

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void resizeEvent(QResizeEvent* event);

private:
    float _minDistance;
    float _maxDistance;
    float _distance;
    float _angle;

    QColor _color;
    QVector<QPainterPath> _paths;
};

#endif

#ifndef __MAP_WIDGET_H__
#define __MAP_WIDGET_H__

#include <QWidget>
#include <QImage>
#include <QSize>
#include <QPoint>
#include <QTransform>

#include <nav_msgs/OccupancyGrid.h>
#include <geometry_msgs/PoseStamped.h>

class MapWidget : public QWidget
{
public:
    MapWidget(QWidget* parent = 0);
    virtual ~MapWidget(void);

    void setMap(const nav_msgs::OccupancyGrid& map);
    void setPose(const geometry_msgs::PoseStamped& pose);

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

private:
    QImage* _image;
    QSize _size;
    QPoint _pos;
    QPoint _origin;
    float _resolution; // m/cell
    float _shownLength;
    QRect _roi;
    QPixmap _robotImg;
    QTransform _transformImg;
};

#endif

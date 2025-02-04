#include "MapWidget.h"
#include "Quaternion.h"

#include <QPainter>
#include <QWheelEvent>
#include <QDebug>

MapWidget::MapWidget(QWidget* parent)
    : QWidget(parent),
      _image(0),
      _resolution(1.0),
      _shownLength(3.0),
      _robotImg(":/ultrasound/enterprise.png")
{

}

MapWidget::~MapWidget(void)
{
    delete _image;
}

void MapWidget::setMap(const nav_msgs::OccupancyGrid& map)
{
    _size = QSize(map.info.width, map.info.height);
    _resolution = map.info.resolution;
    _origin = QPoint(-map.info.origin.position.x / _resolution,
                     -map.info.origin.position.y / _resolution);

    if (!_image || _size != _image->size())
    {
        delete _image;
        _image = new QImage(_size, QImage::Format_RGB888);
    }

    std::vector<int8_t>::const_iterator cell(map.data.begin());
    const QRgb white = QColor(Qt::white).rgb();
    const QRgb black = QColor(Qt::black).rgb();
    const QRgb gray  = QColor(Qt::gray).rgb();

    for (int row = 0; row < _image->height(); ++row)
    {
        for (int col = 0; col < _image->width(); ++col, ++cell)
        {
            switch (*cell)
            {
            case 0:
                _image->setPixel(col, row, white);
                break;

            case -1:
                _image->setPixel(col, row, gray);
                break;

            default:
                _image->setPixel(col, row, black);
                 break;
            }
        }
    }

    *_image = _image->mirrored();

    this->update();
}

void MapWidget::setPose(const geometry_msgs::PoseStamped& pose)
{
    _pos = QPoint(pose.pose.position.x / _resolution, pose.pose.position.y / _resolution);
    Quaternion quat(pose.pose.orientation.x,
                    pose.pose.orientation.y,
                    pose.pose.orientation.z,
                    pose.pose.orientation.w);

    _transformImg.reset();
//    _transformImg.translate(_pos.x(), _pos.y());
    _transformImg.rotate(-quat.yaw() / M_PI * 180.0 + 90.0);
//    qDebug() << "yaw = " << quat.yaw() / M_PI * 180.0;
    this->update();
}

void MapWidget::paintEvent(QPaintEvent*)
{
    if (!_image)
        return;

//    QPainter imgPainter(_image);

    const QPoint r(_shownLength / _resolution, _shownLength / _resolution);
    _roi.setTopLeft(_origin + _pos - r);
    _roi.setBottomRight(_origin + _pos + r);

    QPainter painter(this);
//    QImage map = _image->transformed(_transformImg);
    painter.drawImage(this->rect(), *_image, _roi);
//    painter.drawImage(this->rect(), map, _roi);

    QBrush brush(Qt::SolidPattern);
    brush.setColor(Qt::red);
    painter.setBrush(brush);
//    painter.drawEllipse(this->rect().center(), 5, 5);
//    painter.drawPixmap(QRect(this->rect().center() - QPoint(20, 30), this->rect().center() + QPoint(20, 30)), _robotImg, _robotImg.rect());

    QPixmap robotImg = _robotImg.transformed(_transformImg);
    painter.drawPixmap(QRect(this->rect().center() - QPoint(30, 30), this->rect().center() + QPoint(30, 30)), robotImg, robotImg.rect());
}

void MapWidget::wheelEvent(QWheelEvent* event)
{
    const float step = static_cast<float>(event->delta()) * 0.01;

    event->accept();
    _shownLength += step;
    this->update();
}

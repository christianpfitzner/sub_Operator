#include "ImageWidget.h"

#include <QPainter>
#include <QPoint>
#include <QTextStream>
#include <QDebug>
#include <QKeyEvent>

#include <ros/ros.h>

void ImageWidget::setImage(const cv::Mat& image, const bool setMinSize)
{
    if (image.type() != CV_8UC3)
    {
        ROS_ERROR_STREAM(__PRETTY_FUNCTION__ << ": image type is not implemented yet.");
        return;
    }

    if (setMinSize)
        this->setMinimumSize(QSize(image.cols, image.rows));

    _image = image;
    this->update();
}

void ImageWidget::paintEvent(QPaintEvent*)
{
    const QColor COL_VALID(0x11, 0xa4, 0x0a, 0xa0);
    const QColor COL_NOT_VALID(Qt::red);

    QPainter painter(this);

    painter.drawImage(this->rect(),
                      QImage(_image.data, _image.cols, _image.rows, _image.step, QImage::Format_RGB888));

    QPen pen(QColor(0x11, 0xa4, 0x0a, 0xa0));
    pen.setWidth(4);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setFont(QFont("Arial", 8));

    for (int i = 0; i < _victimPaths.size(); i++)
    {
        QPen pen(painter.pen());
        pen.setColor(_victims[i].valid ? COL_VALID : COL_NOT_VALID);
        pen.setColor(COL_VALID);
        painter.setPen(pen);
        painter.drawPath(_victimPaths[i]);

        QString info;
        QTextStream stream(&info, QIODevice::WriteOnly);
        stream.setRealNumberPrecision(2);
        stream << _victims[i].id << "\n(" << _victims[i].pose.position.x << " " << _victims[i].pose.position.y
               << " " << _victims[i].pose.position.z << ")";

        painter.drawText(_victimLabels[i], Qt::AlignBottom | Qt::AlignRight, info);
    }
}

void ImageWidget::resizeEvent(QResizeEvent*)
{
    this->calcPaths();
}

void ImageWidget::setVictims(const ohm_perception_msgs::VictimArray& victims)
{
    if (!victims.victims.size())
    {
        _victims.clear();
        this->calcPaths();
        this->update();

        return;
    }


    _victims.clear();

    for (unsigned int i = 0; i < victims.victims.size(); i++)
    {
        _victims.push_back(victims.victims[i]);
    }

    this->calcPaths();
    this->update();
}

void ImageWidget::calcPaths(void)
{
    const int SPACE = 6;
    const float SCALE_X = static_cast<float>(this->width()) / static_cast<float>(_image.cols);
    const float SCALE_Y = static_cast<float>(this->height()) / static_cast<float>(_image.rows);
    const int SPACE_X = static_cast<int>(static_cast<float>(SPACE) * SCALE_X);
//    const int SPACE_Y = static_cast<int>(static_cast<float>(SPACE) * SCALE_Y);
    const int SPACE_Y = SPACE_X;

    _victimPaths.resize(_victims.size());
    QVector<QPainterPath>::iterator path(_victimPaths.begin());

    _victimLabels.resize(_victims.size());
    QVector<QRect>::iterator label(_victimLabels.begin());

    for (QVector<ohm_perception_msgs::Victim>::const_iterator victim(_victims.begin());
         victim < _victims.end();
         ++victim, ++path, ++label)
    {
        const int xLT = static_cast<int>(static_cast<float>(victim->u) * SCALE_X);
        const int yLT = static_cast<int>(static_cast<float>(victim->v) * SCALE_Y);

        const int xLB = xLT;
        const int yLB = static_cast<int>(static_cast<float>(victim->v + victim->height) * SCALE_Y);

        const int xRT = static_cast<int>(static_cast<float>(victim->u + victim->width) * SCALE_X);
        const int yRT = yLT;

        const int xRB = xRT;
        const int yRB = yLB;


        /* calc left line */
        QPainterPath tmp(QPoint(xLT - SPACE_X, yLT));
        tmp.lineTo(xLT, yLT + SPACE_Y);
        tmp.lineTo(xLB, yLB - SPACE_Y);
        tmp.lineTo(xLB - SPACE_X, yLB);

        /* calc bottom line */
        tmp.moveTo(xLB, yLB + SPACE_Y);
        tmp.lineTo(xLB + SPACE_X, yLB);
        tmp.lineTo(xRB - SPACE_X, yRB);
        tmp.lineTo(xRB, yRB + SPACE_Y);

        /* calc right line */
        tmp.moveTo(xRB + SPACE_X, yRB);
        tmp.lineTo(xRB, yRB - SPACE_Y);
        tmp.lineTo(xRT, yRT + SPACE_Y);
        tmp.lineTo(xRT + SPACE_X, yRT);

        /* calc top line */
        tmp.moveTo(xRT, yRT - SPACE_Y);
        tmp.lineTo(xRT - SPACE_X, yRT);
        tmp.lineTo(xLT + SPACE_X, yLT);
        tmp.lineTo(xLT, yLT - SPACE_Y);

        *path = tmp;
        *label = QRect(QPoint(xLT, yLT), QPoint(xRB - SPACE_X, yRB));
    }
}

void ImageWidget::keyPressEvent(QKeyEvent* event)
{

}

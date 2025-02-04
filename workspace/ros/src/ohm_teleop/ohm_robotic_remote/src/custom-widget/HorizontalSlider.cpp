#include "HorizontalSlider.h"

#include <QPainter>
#include <QRect>
#include <QDebug>

HorizontalSlider::HorizontalSlider(QWidget* parent)
    : QWidget(parent),
      _min(0),
      _max(100),
      _value(1),
      _color(Qt::blue)
{

}

void HorizontalSlider::setMinimum(const int min)
{
    _min = min;

    if (min > _value)
        this->setValue(min);
}

void HorizontalSlider::setMaximum(const int max)
{
    _max = max;

    if (max < _value)
        this->setValue(max);
}

void HorizontalSlider::setValue(const int value)
{
    if (value < _min || value > _max)
        return;

    _value = value;
    this->update();
}

void HorizontalSlider::setText(const QString& text)
{
    _text = text;
    this->update();
}

void HorizontalSlider::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QPen pen;
    QBrush brush(Qt::SolidPattern);

    /* draw slider */
    const float relation = static_cast<float>(_value) / static_cast<float>(_max - _min);

    QRect left(0, 0, static_cast<int>(relation * static_cast<float>(this->width())), this->height());
    QRect right(QPoint(left.width(), 0), QPoint(this->width(), this->height()));

    pen.setColor(_color);
    brush.setColor(_color);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawRect(left);

    pen.setColor(_color.darker(400));
    brush.setColor(_color.darker(400));
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawRect(right);

    /* draw text */
    QFont font("United Federation of Planets.", this->height() >> 1);
    painter.setFont(font);
    pen.setColor(Qt::black);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawText(this->rect(), Qt::AlignCenter, _text);
    painter.drawText(0, 0, this->width(), this->height(), Qt::AlignCenter, _text);
}

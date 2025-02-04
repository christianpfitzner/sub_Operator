#include "ScrollBar.h"

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

namespace {
const int WIDTH = 24;
const int RADIUS = 12;
}

ScrollBar::ScrollBar(QWidget* parent)
    : QWidget(parent),
      _rectSlider(0, 0, WIDTH, 0),
      _highlighted(false),
      _pressed(false),
      _posSlide(0.0)
{
    this->setFixedWidth(WIDTH);
    this->setMouseTracking(true);
}

void ScrollBar::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QBrush brush(_color.darker());

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(brush);
    painter.setPen(Qt::NoPen);
    painter.drawRect(this->rect());

    brush.setColor(!this->isEnabled() ? _color.lighter() : _pressed ? _color.darker(100) : _highlighted ? _color.lighter() : _color);
    painter.setBrush(brush);
    painter.drawRoundedRect(_rectSlider, RADIUS, RADIUS);
}

void ScrollBar::resizeEvent(QResizeEvent*)
{
    _rectSlider = QRect(0, static_cast<int>(static_cast<float>(this->height()) * _posSlide), WIDTH, this->height() >> 3);
}

void ScrollBar::mouseMoveEvent(QMouseEvent* event)
{
    if (!this->isEnabled())
        return;

    if (_pressed)
    {
        const int diff = event->pos().y() - _posMouse.y();
        float posSlide = static_cast<float>(_rectSlider.top()) / static_cast<float>(this->height() - _rectSlider.height());

        _posMouse = event->pos();

        if (_rectSlider.top() + diff < 0)
        {
            _rectSlider = QRect(0, 0, WIDTH, _rectSlider.height());
            posSlide = 0.0;
        }
        else if (_rectSlider.bottom() + diff > this->rect().bottom())
        {
            _rectSlider = QRect(0, this->height() - _rectSlider.height(), WIDTH, _rectSlider.height());
            posSlide = 1.0;
        }
        else
        {
            _rectSlider.adjust(0, diff, 0, diff);
        }


        if (_posSlide != posSlide)
        {
            _posSlide = posSlide;
            this->update();
            emit this->slideChanged(_posSlide);
        }

        return;
    }

    /* check if inside */
    if (event->pos().y() >= _rectSlider.y() && event->pos().y() <= _rectSlider.bottom())
    {
        if (_highlighted)
            return;

        _highlighted = true;
        this->update();

        return;
    }

    if (_highlighted)
    {
        _highlighted = false;
        this->update();
    }
}

void ScrollBar::mousePressEvent(QMouseEvent* event)
{
    if (!this->isEnabled() || !_highlighted)
        return;

    _pressed = true;
    _posMouse = event->pos();
    this->update();
}

void ScrollBar::mouseReleaseEvent(QMouseEvent*)
{
    if (!_pressed)
        return;

    _pressed = false;
    this->update();
}

void ScrollBar::leaveEvent(QEvent*)
{
    if (!_highlighted)
        return;

    _highlighted = false;
    this->update();
}

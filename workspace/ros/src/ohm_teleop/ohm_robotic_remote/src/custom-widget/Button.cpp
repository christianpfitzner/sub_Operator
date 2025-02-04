#include "Button.h"

#include <cmath>

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QDebug>

Button::Button(QWidget* parent, const QColor& color, const Type type)
    : AbstractButton(parent),
      _points(Count),
      _mouseIn(false),
      _elipsePar(2, 1.0),
      _color(color),
      _type(type)
{
    this->setMouseTracking(true);
}

void Button::setType(const Type type)
{
    _type = type;
    this->update();
}

void Button::setText(const QString& text)
{
    _text = text;
    this->update();
}

void Button::setColor(const QColor& color)
{
    _color = color;
    this->update();
}

void Button::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QBrush brush(!this->isEnabled() ? _color.darker(400) : !_clickable ? _color : _pressed ? _color.darker() : _mouseIn ? _color.lighter() : _color);
    QPen pen(brush.color());
    pen.setWidth(1);
    QFont font("United Federation of Planets.", 12);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setFont(font);

    switch (_type)
    {
    case Normal:
        painter.drawRect(0, 0, this->width() - 1, this->height() - 1);
        break;

    case Right:
        {
            QPainterPath path(_points[Center]);
            path.arcTo(QRect(_points[LT], _points[RB]), 270.0, 180.0);
            painter.drawPath(path);
        }
        break;

    case Left:
        {
            QPainterPath path(_points[Center]);
            path.arcTo(QRect(_points[LT], _points[RB]), 90.0, 180.0);
            painter.drawPath(path);
        }
        break;

    case Triangle:
        {
            QPainterPath path(QPoint(0, 0));
            path.lineTo(QPoint(this->width() - 1, (this->height() - 1) >> 1));
            path.lineTo(QPoint(0, this->height() - 1));
            painter.drawPath(path);
        }

    default:
        break;
    }

    /* draw icon */
    if (!_icon.isNull())
    {
        QRect target(_icon.rect());
        target.moveCenter(QPoint(target.center().x() + 6, this->rect().center().y()));
        painter.drawPixmap(target, _icon, _icon.rect());
    }

    pen.setColor(Qt::black);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawText(this->rect(), Qt::AlignCenter, _text);
}

void Button::mousePressEvent(QMouseEvent*)
{
    if (!this->isEnabled() || !_clickable || !_mouseIn)
        return;

    if (_checkable && _pressed)
    {
        _pressed = false;
        this->released();
        this->update();
        return;
    }

    _pressed = true;
    this->pressed();
    this->update();
}

void Button::mouseReleaseEvent(QMouseEvent*)
{
    if (!this->isEnabled() || !_clickable ||_checkable)
        return;

    if (_mouseIn && _pressed)
    {
        emit this->released();
        emit this->clicked();
    }

    _pressed = false;
    this->update();
}

void Button::mouseMoveEvent(QMouseEvent* event)
{
    if (!this->isEnabled() || !_clickable)
        return;

    QPoint pos(event->pos());
    bool mouseIn = false;

    switch (_type)
    {
    case Normal:
        mouseIn = pos.x() >= 0 && pos.y() >= 0 && pos.x() < this->width() && pos.y() < this->height();
        break;

    case Right:
        pos -= QPoint(0, this->height() >> 1);
        mouseIn = static_cast<float>(pos.x()) * static_cast<float>(pos.x()) * _elipsePar[0] +
                  static_cast<float>(pos.y()) * static_cast<float>(pos.y()) * _elipsePar[1] <= 1.0;
        break;

    case Left:
        pos -= QPoint(this->width() - 1, this->height() >> 1);
        mouseIn = static_cast<float>(pos.x()) * static_cast<float>(pos.x()) * _elipsePar[0] +
                  static_cast<float>(pos.y()) * static_cast<float>(pos.y()) * _elipsePar[1] <= 1.0;

    case Triangle:
        {
            QPointF v(QPointF(0, 0) - QPointF(this->width() - 1, (this->height() - 1) >> 1));
            QPointF n(v.y(), -v.x());
            n /= std::sqrt(n.x() * n.x() + n.y() * n.y());
            float d = n.x() * 0.0f + n.y() * 0.0f;

            mouseIn = n.x() * pos.x() + n.y() * pos.y() - d > 0.0f;

            v = QPointF(0, this->height() - 1) - QPointF(this->width() - 1, (this->height() - 1) >> 1);
            n = QPointF(v.y(), -v.x());
            n /= std::sqrt(n.x() * n.x() + n.y() * n.y());
            d = n.x() * 0.0f + n.y() * static_cast<float>((this->height() - 1));

            mouseIn &= n.x() * pos.x() + n.y() * pos.y() - d < 0.0f;
        }

        break;

    default:
        return;
    }


    if (mouseIn && !_mouseIn)
    {
        _mouseIn = true;
        this->update();
        return;
    }

    if (!mouseIn && _mouseIn)
    {
        _mouseIn = false;
        this->update();
        return;
    }
}

void Button::resizeEvent(QResizeEvent*)
{
    switch (_type)
    {
    case Normal:
    default:
        return;

    case Right:
        _points[LT] = QPoint(-(this->width() - 1), 0);
        _points[RB] = QPoint(this->width() - 1, this->height() - 1);
        _points[Center] = QPoint(0, this->height());
        _points[Center] /= 2;

        break;

    case Left:
        _points[LT] = QPoint(0, 0);
        _points[RB] = QPoint((this->width() << 1) - 1, this->height() - 1);
        _points[Center] = QPoint(this->width(), (this->height() >> 1) - 1);

        break;
    }

    _elipsePar[0] = 1.0 / (static_cast<float>(this->width()) * static_cast<float>(this->width()));
    _elipsePar[1] = 1.0 / (static_cast<float>(this->height() >> 1) * static_cast<float>(this->height() >> 1));
}

void Button::leaveEvent(QEvent*)
{
    if (!_checkable)
        _pressed = false;

    _mouseIn = false;
    this->update();
}

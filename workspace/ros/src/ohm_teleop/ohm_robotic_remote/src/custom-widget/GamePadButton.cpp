#include "GamePadButton.h"

#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QBitmap>

GamePadButton::GamePadButton(QWidget* parent)
    : QWidget(parent),
      _pressed(false),
      _mousePressed(false),
      _highlighted(false)
{

}

GamePadButton::GamePadButton(const QString& image, const QString& imagePressed, QWidget* parent)
    : QWidget(parent),
      _pressed(false),
      _mousePressed(false),
      _highlighted(false),
      _background(image),
      _backgroundPressed(imagePressed)
{

}

void GamePadButton::setImages(const QString& image, const QString& imagePressed)
{
    _background = QPixmap(image);
    _backgroundPressed = QPixmap(imagePressed);
    this->update();
}

void GamePadButton::setPressed(const bool pressed)
{
    _pressed = pressed;
    this->update();
}

void GamePadButton::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    if (_pressed || _mousePressed)
        painter.drawPixmap(_backgroundPressed.rect(), _backgroundPressed, this->rect());
    else
        painter.drawPixmap(_background.rect(), _background, this->rect());

    if (_highlighted)
    {
        QBrush brush(QColor(0, 106, 200, 80));
        QPen pen(Qt::transparent);
        pen.setWidth(0);

        if (_pressed || _mousePressed)
            brush.setTexture(_backgroundPressed.mask());
        else
            brush.setTexture(_background.mask());

        painter.setBrush(brush);
        painter.setPen(pen);
        painter.drawRect(this->rect());
    }
}

void GamePadButton::mousePressEvent(QMouseEvent*)
{
    _mousePressed = true;
    _pressed = true;
    this->update();
    emit this->pressed();
}

void GamePadButton::mouseReleaseEvent(QMouseEvent*)
{
    if (!_mousePressed)
        return;

    _mousePressed = false;
    _pressed = false;
    this->update();
    emit this->released();
    emit this->clicked();
}

void GamePadButton::enterEvent(QEvent*)
{
    _highlighted = true;
    this->update();
}

void GamePadButton::leaveEvent(QEvent*)
{
    _highlighted = false;
    this->update();
}

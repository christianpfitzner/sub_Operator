#include "StarTrekLayout.h"

#include <QWidget>
#include <QPainter>
#include <QDebug>

namespace {
const unsigned int SPACE = 6;
const unsigned int SEPERATOR = 32;
const unsigned int CHAR_WIDTH = 48;
}

StarTrekLayout::StarTrekLayout(const Direction direction, QWidget* parent)
    : QWidget(parent),
      _direction(direction),
      _color(Qt::red),
      _mainWidget(0),
      _paths(3)
{
    this->setPalette(QPalette(Qt::black));
}

void StarTrekLayout::setMainWidget(QWidget* widget)
{
    if (widget == _mainWidget)
        return;

    _mainWidget = widget;
    _mainWidget->setParent(this);
    this->resizeEvent(0);
}

void StarTrekLayout::addSideWidget(QWidget* widget)
{
    widget->setParent(this);
    _sideWidgets.push_back(widget);
    _sideRects.push_back(widget->rect());
    this->resize(this->size());
}

void StarTrekLayout::addSeparator(void)
{
    _sideWidgets.push_back(0);
    _sideRects.push_back(QRect());
    this->update();
}

void StarTrekLayout::setText(const QString& text)
{
    _text = text;
    this->update();
}

void StarTrekLayout::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(_color);
    pen.setWidth(0);
    painter.setPen(pen);
    QBrush brush(_color);
    painter.setBrush(brush);

    for (int i = 0; i < _paths.size(); i++)
        painter.drawPath(_paths[i]);

    QFont font("United Federation of Planets.", this->height() >> 4);
    painter.setFont(font);
    painter.drawText(_posText, Qt::AlignCenter, _text);

    for (int i = 0; i < _sideWidgets.size(); i++)
    {
        if (_sideWidgets[i])
        {
            _sideWidgets[i]->setGeometry(_sideRects[i]);
        }
        else
        {
            painter.drawRect(_sideRects[i]);
        }
    }
}

void StarTrekLayout::resizeEvent(QResizeEvent* event)
{
    const int w1_4  = this->width() >> 2;

    const int h1_8  = this->height() >> 3;
    const int h1_16 = this->height() >> 4;
    const int h1_32 = this->height() >> 5;

    switch (_direction)
    {
    case TopToBottom:
        {
            /* main widget */
            if (_mainWidget)
                _mainWidget->setGeometry(w1_4 + SPACE, h1_8 + SPACE, this->width() - w1_4 - SPACE, this->height() - h1_8 - SPACE);

            /* top right */
            QPainterPath path(QPoint(this->width() - 1 - h1_16, 0));
            path.lineTo(this->width() - 1 - h1_32, 0);
            path.quadTo(this->width() - 1, 0, this->width() - 1, h1_32);
            path.quadTo(this->width() - 1, h1_16, this->width() - 1 - h1_32, h1_16);
            path.lineTo(this->width() - 1 - h1_16, h1_16);
            path.lineTo(this->width() - 1 - h1_16, 0);
            _paths[1] = path;

            /* text */
            _posText = QRect(path.elementAt(0).x - _text.count() * h1_16, 0, _text.count() * h1_16, h1_16);

            /* top left */
            path = QPainterPath(QPoint(0, h1_8));
            path.quadTo(0, 0, h1_8, 0);
            path.lineTo(_posText.x(), 0.0);
            path.lineTo(_posText.x(), _posText.height());
            path.lineTo(h1_16 + w1_4 + SPACE, h1_16);
            path.quadTo(w1_4, h1_16, w1_4, h1_8);
            path.lineTo(0, h1_8);
            path.setFillRule(Qt::WindingFill);
            _paths[0] = path;

            /* side widgets */
            /* here is a bug fix me!!! */
            for (int i = 0; i < _sideWidgets.size(); i++)
            {
                const int height = _sideWidgets[i] ? _sideWidgets[i]->minimumHeight() : SEPERATOR;
                const int y = i ? _sideRects[i - 1].bottom() + SPACE : h1_8 + SPACE;

                _sideRects[i] = QRect(0, y, w1_4, height);
            }

            /* bottom left */
            path = QPainterPath(QPoint(0, _sideRects.size() ? _sideRects.last().y() + _sideRects.last().height() + SPACE : h1_8));
            path.lineTo(w1_4, path.elementAt(0).y);
            path.lineTo(path.elementAt(1).x, this->height() - 1);
            path.lineTo(0, this->height() - 1);
            path.lineTo(path.elementAt(0).x, path.elementAt(0).y);
            _paths[2] = path;
        }
        break;

    case BottomToTop:
        {
            /* main widget */
            if (_mainWidget)
                _mainWidget->setGeometry(w1_4 + SPACE + SPACE, 0, this->width() - w1_4 - SPACE - SPACE - h1_32, this->height() - h1_16 - SPACE - SPACE);

            /* bottom right */
            QPainterPath path(QPoint(this->width() - 1 - h1_16 - h1_32, this->height() - 1));
            path.lineTo(this->width() - 1 - h1_32, path.elementAt(0).y);
            path.quadTo(this->width() - 1, path.elementAt(1).y, this->width() - 1, path.elementAt(1).y - h1_32);
            path.quadTo(this->width() - 1, this->height() - 1 - h1_16, path.elementAt(1).x, this->height() - 1 - h1_16);
            path.lineTo(path.elementAt(0).x, this->height() - 1 - h1_16);
            path.lineTo(path.elementAt(0).x, path.elementAt(0).y);
            _paths[1] = path;

            /* text */
            _posText = QRect(path.elementAt(0).x - _text.count() * h1_16, this->height() - 1 - h1_16, _text.count() * h1_16, h1_16);

            /* bottom left */
            path = QPainterPath(QPoint(0, this->height() - 1 - h1_8));
            path.quadTo(0, this->height() - 1, h1_8, this->height() - 1);
            path.lineTo(_posText.x(), this->height() - 1);
            path.lineTo(_posText.x(), path.elementAt(2).y - _posText.height());
            path.lineTo(h1_16 + w1_4 + SPACE, this->height() - 1 - h1_16);
            path.quadTo(w1_4, this->height() - 1 - h1_16, w1_4, this->height() - 1 - h1_8);
            path.lineTo(0, path.elementAt(0).y);
            path.setFillRule(Qt::WindingFill);
            _paths[0] = path;

            /* side widgets */
            for (int i = 0; i < _sideWidgets.size(); i++)
            {
                const int height = _sideWidgets[i] ? _sideWidgets[i]->minimumHeight() : SEPERATOR;
                const int y = i ? _sideRects[i - 1].y() - SPACE - height : this->height() - height - h1_8 - SPACE;

                _sideRects[i] = QRect(0, y, w1_4, height);
            }

            /* top left */
            path = QPainterPath(QPoint(0, _sideWidgets.size() ? _sideRects.last().y() - SPACE: this->height() - 1 - h1_8 - SPACE));
            path.lineTo(w1_4, path.elementAt(0).y);
            path.lineTo(path.elementAt(1).x, 0);
            path.lineTo(0, 0);
            path.lineTo(path.elementAt(0).x, path.elementAt(0).y);
            _paths[2] = path;
        }
        break;

    default:
        break;
    }
}

void StarTrekLayout::setColor(const QColor& color)
{
    _color = color;
    this->update();
}

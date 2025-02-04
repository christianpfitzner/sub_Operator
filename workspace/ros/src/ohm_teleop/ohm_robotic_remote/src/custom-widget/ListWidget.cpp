#include "ListWidget.h"
#include "ScrollBar.h"

#include <QPainter>
#include <QDebug>

namespace {
const unsigned int ITEM_HEIGHT = 16;
const unsigned int SPACE = 6;
}

ListWidget::ListWidget(QWidget* parent)
    : QWidget(parent),
      _model(0),
      _rects(CountRect),
      _itemsToShow(0),
      _scrollBar(new ScrollBar(this)),
      _row(-1)
{
    _scrollBar->setColor(Qt::red);

    this->connect(_scrollBar, SIGNAL(slideChanged(const float)), this, SLOT(slideChanged(const float)));
}

ListWidget::~ListWidget(void)
{
    delete _model;
}

void ListWidget::paintEvent(QPaintEvent* event)
{
    if (!_model)
        return;

    QPainter painter(this);

    QFont font("United Federation of Planets.", ITEM_HEIGHT);
    painter.setFont(font);
    QPen pen;

    QVector<const ListWidgetItem*> items(_model->get(_scrollBar->posSlide(), _itemsToShow));
    QVector<const ListWidgetItem*>::const_iterator item(items.begin());
    QVector<QRect> rects(_rects);

    while (item < items.end())
    {
        switch ((*item)->type())
        {
        case ListWidgetItem::None:
            pen.setColor(Qt::black);
            break;
        case ListWidgetItem::ValidVictim:
            pen.setColor(Qt::darkGreen);
            break;
        case ListWidgetItem::VictimRequest:
            pen.setColor(Qt::darkGreen);
            break;
        case ListWidgetItem::Hazat:
            pen.setColor(Qt::green);
            break;
        case ListWidgetItem::Qr:
            pen.setColor(Qt::blue);
            break;
        default:
            pen.setColor(Qt::black);
            break;
        }

        painter.setPen(pen);
        painter.drawText(rects[Time], Qt::AlignLeft | Qt::AlignVCenter, (*item)->time().toString());
        painter.drawText(rects[Type], Qt::AlignLeft | Qt::AlignVCenter, ListWidgetItem::typeName((*item)->type()));
        painter.drawText(rects[Text], Qt::AlignLeft | Qt::AlignVCenter, (*item)->text());
        painter.drawText(rects[Meta], Qt::AlignLeft | Qt::AlignVCenter, (*item)->meta());
        item++;

        for (int i = 0; i < Meta; i++)
            rects[i].adjust(0, ITEM_HEIGHT, 0, ITEM_HEIGHT);
    }

    _scrollBar->setGeometry(_rects[Bar]);

    if (_row >= 0)
    {
        QColor color(_color);
        color.setAlpha(100);
        QBrush brush(color);
        painter.setPen(Qt::NoPen);
        painter.setBrush(brush);
        painter.drawRect(QRect(0, ITEM_HEIGHT * _row, this->width() - _rects[Bar].width() - SPACE, ITEM_HEIGHT));
    }
}

void ListWidget::resizeEvent(QResizeEvent* event)
{
    const int w2_8  = this->width() >> 2;
    const int w1_16 = this->width() >> 4;

    _rects[Time] = QRect(0, 0, w2_8, ITEM_HEIGHT);
    _rects[Type] = QRect(_rects[Time].width(), 0, w2_8 + w1_16, ITEM_HEIGHT);
    _rects[Text] = QRect(_rects[Type].x() + _rects[Type].width(), 0, w2_8, ITEM_HEIGHT);
    _rects[Meta] = QRect(_rects[Text].x() + _rects[Text].width(), 0, w2_8 - w1_16 - _scrollBar->width(), ITEM_HEIGHT);
    _rects[Bar]  = QRect(this->width() - _scrollBar->width(), 0, _scrollBar->width(), this->height());

    _itemsToShow = this->height() / ITEM_HEIGHT + 1;
    _scrollBar->setEnabled(_model ? _itemsToShow < _model->items().size() : false);
}

void ListWidget::slideChanged(const float slide)
{
    this->update();
}

void ListWidget::setColor(const QColor& color)
{
    _scrollBar->setColor(color);
    _color = color;
}

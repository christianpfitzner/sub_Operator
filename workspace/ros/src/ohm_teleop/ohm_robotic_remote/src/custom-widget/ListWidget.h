#ifndef __LIST_WIDGET__
#define __LIST_WIDGET__

#include <QWidget>
#include <QColor>
#include <QVector>

#include "ListWidgetModel.h"
#include "ListWidgetItem.h"

class ScrollBar;

class ListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ListWidget(QWidget* parent = 0);
    virtual ~ListWidget(void);

    void setColor(const QColor& color);
    void setModel(ListWidgetModel* model) { _model = model; this->update(); }
    ListWidgetModel* model(void) { return _model; }

public slots:
    inline void addItem(ListWidgetItem* item) { if (!_model) _model = new ListWidgetModel(); _model->addItem(item); this->update(); }
    inline void removeItem(const int index) { _model->removeItem(index); this->update(); }
    inline void selectRow(const int row) { _row = row; this->update(); }
    void slideChanged(const float slide);

signals:
    void currentRowChanged(const int row);

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void resizeEvent(QResizeEvent* event);

private:

    enum Rects {
        Time = 0,
        Type,
        Text,
        Meta,
        Bar,
        CountRect
    };

    ListWidgetModel* _model;
    QVector<QRect> _rects;
    int _itemsToShow;
    ScrollBar* _scrollBar;
    int _row;
    QColor _color;
};

#endif

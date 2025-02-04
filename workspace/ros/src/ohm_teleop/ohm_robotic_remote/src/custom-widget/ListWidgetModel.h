#ifndef __LIST_WIDGET_MODEL__
#define __LIST_WIDGET_MODEL__

#include "ListWidgetItem.h"

#include <QList>
#include <QVector>

class ListWidgetModel
{
public:
    ListWidgetModel(void) { }
    ~ListWidgetModel(void);

    void addItem(ListWidgetItem* item);
    QVector<const ListWidgetItem*> getFirstX(const int x) const;
    QVector<const ListWidgetItem*> get(const float slide, const int shownItems);
    const QList<ListWidgetItem*> items(void) const { return _items; }
    void removeItem(const int index);

private:
    QList<ListWidgetItem*> _items;
};

#endif

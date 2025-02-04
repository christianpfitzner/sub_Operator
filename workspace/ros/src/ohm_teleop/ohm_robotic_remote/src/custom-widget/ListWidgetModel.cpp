#include "ListWidgetModel.h"

#include <QDebug>

ListWidgetModel::~ListWidgetModel(void)
{
    qDeleteAll(_items);
}

void ListWidgetModel::addItem(ListWidgetItem* item)
{
    _items.push_front(item);
}

QVector<const ListWidgetItem*> ListWidgetModel::getFirstX(const int x) const
{
    const int end = x > _items.size() ? _items.size() : x;
    QList<ListWidgetItem*>::const_iterator item(_items.begin());
    QVector<const ListWidgetItem*> out;

    for (int i = 0; i < end; i++)
        out.push_back(*item++);

    return out;
}

QVector<const ListWidgetItem*> ListWidgetModel::get(const float slide, const int shownItems)
{
    const int begin = static_cast<int>(static_cast<float>(_items.size()) * slide);
    const int end = begin + shownItems > _items.size() ? _items.size() : begin + shownItems;

    QList<ListWidgetItem*>::const_iterator item(_items.begin() + begin);
    QVector<const ListWidgetItem*> out;

    for (int i = begin; i < end; i++)
        out.push_back(*item++);

    return out;
}

void ListWidgetModel::removeItem(const int index)
{
    _items.removeAt(index);
}

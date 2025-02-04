#include "TextSpinBox.h"

TextSpinBox::TextSpinBox(QWidget* parent, const QColor& color)
    : AbstractSpinBox(parent, color),
      _index(-1)
{

}

void TextSpinBox::setCurrentIndex(const unsigned int index)
{
    if (static_cast<int>(index) >= _items.count())
        return;

    _index = index;
    _ui->_buttonMiddle->setText(_items[_index]);
    emit this->indexChanged(_index);
    emit this->indexChanged(_items[_index]);
}

void TextSpinBox::addItem(const QString& text, const QVariant& data)
{
    _items.push_back(text);
    _datas.push_back(data);

    if (_items.count() == 1)
    {
        this->setCurrentIndex(0);
    }
}

void TextSpinBox::increment(void)
{
    if (!_items.count())
    {
        _index = -1;
        _ui->_buttonMiddle->setText("");
        return;
    }

    if (_index + 1 >= _items.count())
    {
        this->setCurrentIndex(0);
        return;
    }

    this->setCurrentIndex(_index + 1);
}

void TextSpinBox::decrement(void)
{
    if (!_items.count())
    {
        _index = -1;
        _ui->_buttonMiddle->setText("");
        return;
    }

    if (_index - 1 < 0)
    {
        this->setCurrentIndex(_items.count() - 1);
        return;
    }

    this->setCurrentIndex(_index - 1);
}


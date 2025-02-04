#include "IntSpinBox.h"

IntSpinBox::IntSpinBox(QWidget* parent, const QColor& color)
    : AbstractSpinBox(parent, color),
      _min(0),
      _max(100),
      _value(1)
{
    this->setValue(1);
}

void IntSpinBox::setMin(const int min)
{
    if (min > _value)
        this->setValue(min);

    _min = min;
}

void IntSpinBox::setMax(const int max)
{
    if (max < _value)
        this->setValue(max);

    _max = max;
}


void IntSpinBox::setValue(const int value)
{
    if (value < _min || value > _max)
        return;

    _value = value;
    _ui->_buttonMiddle->setText(QString::number(_value));
    emit this->valueChanged(_value);
}

void IntSpinBox::increment(void)
{
    this->setValue(_value + 1);
}

void IntSpinBox::decrement(void)
{
    this->setValue(_value - 1);
}

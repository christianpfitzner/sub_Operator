#include "Co2Widget.h"
#include "ui_Co2Widget.h"

Co2Widget::Co2Widget(QWidget* parent)
    : QWidget(parent),
      _ui(new Ui::Co2Widget)
{
    _ui->setupUi(this);
    _ui->_slider->setText("CO2");
    _ui->_slider->setMinimum(0);
    _ui->_slider->setMaximum(400);
}

void Co2Widget::setCo2Value(const int value)
{
    _ui->_slider->setValue(value);
}

void Co2Widget::setColor(const QColor& color)
{
    _ui->_slider->setColor(color);
}

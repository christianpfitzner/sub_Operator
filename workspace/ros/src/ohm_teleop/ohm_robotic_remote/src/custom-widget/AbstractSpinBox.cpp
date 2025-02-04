#include "AbstractSpinBox.h"

AbstractSpinBox::AbstractSpinBox(QWidget* parent, const QColor& color)
        : QWidget(parent),
          _ui(new Ui::AbstractSpinBox)
{
    _ui->setupUi(this);
    _ui->_buttonRight->setType(Button::Right);
    _ui->_buttonLeft->setType(Button::Left);
    _ui->_buttonMiddle->setType(Button::Normal);
    _ui->_buttonMiddle->setClickable(false);
    this->setColor(color);
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    this->connect(_ui->_buttonRight, SIGNAL(clicked()), this, SLOT(increment()));
    this->connect(_ui->_buttonLeft , SIGNAL(clicked()), this, SLOT(decrement()));
}

void AbstractSpinBox::setColor(const QColor& color)
{
    _color = color;
    _ui->_buttonRight->setColor(_color);
    _ui->_buttonLeft->setColor(_color);
    _ui->_buttonMiddle->setColor(_color);
}

/*
 * JoyConfigInputWidget.cpp
 *
 *  Created on: 11.02.2015
 *      Author: chris
 */

#include "JoyConfigInputWidget.h"
#include "ui_JoyConfigInputWidget.h"

JoyConfigInputWidget::JoyConfigInputWidget(QWidget* parent) :
    QWidget(parent),
    _ui(new Ui::JoyConfigInputWidget),
    _type(Axis)
{
   _ui->setupUi(this);

   _ui->label_2->setText(QString::number(-1));
   connect(_ui->pushButton,  SIGNAL(released()), this, SLOT(slot_clicked()));

}

JoyConfigInputWidget::~JoyConfigInputWidget(void)
{
   delete _ui;
}


void JoyConfigInputWidget::setText(QString text)
{
   _ui->label->setText(text);
}


void JoyConfigInputWidget::setActive(void)
{
   _ui->pushButton->setFlat(true);
   _ui->pushButton->setText("Cancel");
}

void JoyConfigInputWidget::setInactive(void)
{
   _ui->pushButton->setFlat(false);
   _ui->pushButton->setText("Set");
}


void JoyConfigInputWidget::setAxisOrButton(unsigned int id)
{
   _axis_button_idx = id;
   _ui->label_2->setText(QString::number(id));
   _ui->pushButton->setFlat(false);

}

QString JoyConfigInputWidget::getName(void)
{
   return _ui->label->text();
}

void JoyConfigInputWidget::slot_clicked(void)
{
   emit clicked(_idx);
}

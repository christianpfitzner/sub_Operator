#include "FlatDialog.h"
#include "ui_FlatDialog.h"
#include "GamePadWidget.h"

#include <QKeyEvent>

FlatDialog::FlatDialog(QWidget* parent)
    : QDialog(parent),
      _ui(new Ui::FlatDialog)
{
    _ui->setupUi(this);
    _ui->_buttonReject->setImages(":/gamepad/button-O.png"     , ":/gamepad/button-O-pressed.png");
    _ui->_buttonAccept->setImages(":/gamepad/button-X.png"     , ":/gamepad/button-X-pressed.png");
    _ui->_buttonCancel->setImages(":/gamepad/button-square.png", ":/gamepad/button-square-pressed.png");

    this->connect(_ui->_buttonAccept, SIGNAL(clicked()), this, SLOT(acceptDialog()));
    this->connect(_ui->_buttonReject, SIGNAL(clicked()), this, SLOT(rejectDialog()));
    this->connect(_ui->_buttonCancel, SIGNAL(clicked()), this, SLOT(cancelDialog()));
}

void FlatDialog::setText(const QString& text)
{
    _ui->_labelText->setText(text);
}

void FlatDialog::acceptDialog(void)
{
    this->done(this->result());
}

void FlatDialog::rejectDialog(void)
{
    this->done(-this->result());
}

void FlatDialog::cancelDialog(void)
{
    this->done(0);
}

void FlatDialog::keyPressEvent(QKeyEvent* event)
{
    if (!this->isVisible())
        return;

    switch (event->key())
    {
    case Qt::Key_L:
        this->acceptDialog();
        break;

    case Qt::Key_P:
        this->rejectDialog();
        break;

    case Qt::Key_K:
        this->cancelDialog();
        break;

    default:
        break;
    }
}

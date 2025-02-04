#include "GamePadWidget.h"
#include "ui_GamePadWidget.h"

#include <QPixmap>
#include <QPainter>

namespace {
const unsigned int BUTTONS = 19;
const unsigned int AXES = 27;
}

GamePadWidget::GamePadWidget(QWidget* parent)
    : QWidget(parent),
      _ui(new Ui::GamePadWidget)
{
    _ui->setupUi(this);

    _ui->_buttonUp->setImages(":/gamepad/button-up.png", ":/gamepad/button-up-pressed.png");
    _ui->_buttonLeft->setImages(":/gamepad/button-left.png", ":/gamepad/button-left-pressed.png");
    _ui->_buttonDown->setImages(":/gamepad/button-down.png", ":/gamepad/button-down-pressed.png");
    _ui->_buttonRight->setImages(":/gamepad/button-right.png", ":/gamepad/button-right-pressed.png");
    _ui->_buttonStart->setImages(":/gamepad/button-start.png", ":/gamepad/button-start-pressed.png");
    _ui->_buttonSelect->setImages(":/gamepad/button-select.png", ":/gamepad/button-select-pressed.png");
    _ui->_buttonO->setImages(":/gamepad/button-O.png", ":/gamepad/button-O-pressed.png");
    _ui->_buttonTriangle->setImages(":/gamepad/button-triangle.png", ":/gamepad/button-triangle-pressed.png");
    _ui->_buttonX->setImages(":/gamepad/button-X.png", ":/gamepad/button-X-pressed.png");
    _ui->_buttonSquare->setImages(":/gamepad/button-square.png", ":/gamepad/button-square-pressed.png");
    _ui->_buttonPS->setImages(":/gamepad/button-ps.png", ":/gamepad/button-ps-pressed.png");

    this->connect(_ui->_buttonUp, SIGNAL(pressed()), this, SLOT(buttonEvent()));
    this->connect(_ui->_buttonLeft, SIGNAL(pressed()), this, SLOT(buttonEvent()));
    this->connect(_ui->_buttonDown, SIGNAL(pressed()), this, SLOT(buttonEvent()));
    this->connect(_ui->_buttonRight, SIGNAL(pressed()), this, SLOT(buttonEvent()));
    this->connect(_ui->_buttonStart, SIGNAL(pressed()), this, SLOT(buttonEvent()));
    this->connect(_ui->_buttonSelect, SIGNAL(pressed()), this, SLOT(buttonEvent()));
    this->connect(_ui->_buttonO, SIGNAL(pressed()), this, SLOT(buttonEvent()));
    this->connect(_ui->_buttonTriangle, SIGNAL(pressed()), this, SLOT(buttonEvent()));
    this->connect(_ui->_buttonX, SIGNAL(pressed()), this, SLOT(buttonEvent()));
    this->connect(_ui->_buttonSquare, SIGNAL(pressed()), this, SLOT(buttonEvent()));
    this->connect(_ui->_buttonPS, SIGNAL(pressed()), this, SLOT(buttonEvent()));
}

void GamePadWidget::paintEvent(QPaintEvent*)
{

}

void GamePadWidget::joyEvent(QVector<float>& axis, QVector<bool>& buttons)
{
    if (axis.size() < Count || buttons.size() < Count)
        return;

    _ui->_buttonUp->setPressed(buttons[Up]);
    _ui->_buttonLeft->setPressed(buttons[Left]);
    _ui->_buttonDown->setPressed(buttons[Down]);
    _ui->_buttonRight->setPressed(buttons[Right]);
    _ui->_buttonStart->setPressed(buttons[Start]);
    _ui->_buttonSelect->setPressed(buttons[Select]);
    _ui->_buttonO->setPressed(buttons[O]);
    _ui->_buttonTriangle->setPressed(buttons[Triangle]);
    _ui->_buttonX->setPressed(buttons[X]);
    _ui->_buttonSquare->setPressed(buttons[Square]);
    _ui->_buttonPS->setPressed(buttons[PS]);
}

void GamePadWidget::buttonEvent(void)
{
    QVector<float> axes(AXES, 0.0);
    QVector<bool> buttons(BUTTONS, false);

    buttons[Up]       = _ui->_buttonUp->isPressed();
    buttons[Left]     = _ui->_buttonLeft->isPressed();
    buttons[Down]     = _ui->_buttonDown->isPressed();
    buttons[Right]    = _ui->_buttonRight->isPressed();
    buttons[Start]    = _ui->_buttonStart->isPressed();
    buttons[Select]   = _ui->_buttonSelect->isPressed();
    buttons[O]        = _ui->_buttonO->isPressed();
    buttons[Triangle] = _ui->_buttonTriangle->isPressed();
    buttons[X]        = _ui->_buttonX->isPressed();
    buttons[Square]   = _ui->_buttonSquare->isPressed();
    buttons[PS]       = _ui->_buttonPS->isPressed();

    emit this->sendJoyEvent(axes, buttons);
}

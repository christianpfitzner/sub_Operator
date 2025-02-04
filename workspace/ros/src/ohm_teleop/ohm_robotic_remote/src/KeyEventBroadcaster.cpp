#include "KeyEventBroadcaster.h"
#include "GamePadWidget.h"

#include <QCoreApplication>
#include <QKeyEvent>

KeyEventBroadcaster::KeyEventBroadcaster(QObject* parent)
    : QObject(parent),
      _receiver(0)
{

}

void KeyEventBroadcaster::joyEvent(QVector<float>& axes, QVector<bool>& button)
{
    if (!_receiver || button.size() < GamePadWidget::Count)
        return;

    /*
        Up = 4,
        Down = 6,
        Left = 7,
        Right = 5,
        Start = 3,
        Select = 0,
        Square = 15,
        Triangle = 12,
        X = 14,
        O = 13,
        PS = 16,
        Count = 17
    */

    QVector<QKeyEvent*> events;

    if (button[GamePadWidget::Up])
        events.push_back(new QKeyEvent(QEvent::KeyPress, Qt::Key_W, Qt::NoModifier, "w"));
    if (button[GamePadWidget::Down])
        events.push_back(new QKeyEvent(QEvent::KeyPress, Qt::Key_S, Qt::NoModifier, "s"));
    if (button[GamePadWidget::Left])
        events.push_back(new QKeyEvent(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier, "a"));
    if (button[GamePadWidget::Right])
        events.push_back(new QKeyEvent(QEvent::KeyPress, Qt::Key_D, Qt::NoModifier, "d"));
    if (button[GamePadWidget::Square])
        events.push_back(new QKeyEvent(QEvent::KeyPress, Qt::Key_K, Qt::NoModifier, "k"));
    if (button[GamePadWidget::Triangle])
        events.push_back(new QKeyEvent(QEvent::KeyPress, Qt::Key_O, Qt::NoModifier, "o"));
    if (button[GamePadWidget::X])
        events.push_back(new QKeyEvent(QEvent::KeyPress, Qt::Key_L, Qt::NoModifier, "l"));
    if (button[GamePadWidget::O])
        events.push_back(new QKeyEvent(QEvent::KeyPress, Qt::Key_P, Qt::NoModifier, "p"));


    for (int i = 0; i < events.size(); i++)
        QCoreApplication::postEvent(_receiver, events[i]);

    /* may be i have to delete events */
}

#ifndef __KEY_EVENT_BROADCASTER__
#define __KEY_EVENT_BROADCASTER__

#include <QObject>
#include <QVector>

class QWidget;

class KeyEventBroadcaster : public QObject
{
    Q_OBJECT

public:
    KeyEventBroadcaster(QObject* parent = 0);

    void setReceiver(QWidget* receiver) { _receiver = receiver; }

public slots:
    void joyEvent(QVector<float>& axes, QVector<bool>& button);

private:
    QWidget* _receiver;
};

#endif

#ifndef __GAME_PAD_WIDGET__
#define __GAME_PAD_WIDGET__

#include <QWidget>
#include <QVector>

class QPixmap;

namespace Ui {
class GamePadWidget;
}

class GamePadWidget : public QWidget
{
    Q_OBJECT

public:

    enum Button {
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
    };

    explicit GamePadWidget(QWidget* parent = 0);

public slots:
    void joyEvent(QVector<float>& axes, QVector<bool>& buttons);

signals:
    void sendJoyEvent(QVector<float>& axes, QVector<bool>& buttons);

protected:
    void paintEvent(QPaintEvent* event);

private slots:
    void buttonEvent(void);

private:

    Ui::GamePadWidget* _ui;
};

#endif

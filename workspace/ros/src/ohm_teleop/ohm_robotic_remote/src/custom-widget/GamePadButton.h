#ifndef __GAME_PAD_BUTTON__
#define __GAME_PAD_BUTTON__

#include <QWidget>
#include <QPixmap>

class QString;

class GamePadButton : public QWidget
{
    Q_OBJECT

public:
    explicit GamePadButton(QWidget* parent = 0);
    explicit GamePadButton(const QString& image, const QString& imagePressed, QWidget* parent = 0);

    void setImages(const QString& image, const QString& imagePressed);
    bool isPressed(void) const { return _pressed; }

public slots:
    void setPressed(const bool pressed = true);

signals:
    void pressed(void);
    void released(void);
    void clicked(void);

protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);

private:
    bool _pressed;
    bool _mousePressed;
    bool _highlighted;
    QPixmap _background;
    QPixmap _backgroundPressed;
};

#endif

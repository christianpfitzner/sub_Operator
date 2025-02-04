#ifndef __GAME_PAD_AXIS__
#define __GAME_PAD_AXIS__

#include <QWidget>
#include <QPixmap>

class QString;

class GamePadAxis : public QWidget
{
    Q_OBJECT

public:
    explicit GamePadAxis(QWidget* parent = );

    void setImage(const QString& image);

public slots:
    void setXY(const float x, const float y);

protected:
    void paintEvent(QPaintEvent* event);

private:
    QPixmap _image;
    float _x;
    float _y;
};

#endif

#ifndef __SCROLL_BAR__
#define __SCROLL_BAR__

#include <QWidget>
#include <QColor>
#include <QRect>
#include <QPoint>

class ScrollBar : public QWidget
{
    Q_OBJECT

public:
    explicit ScrollBar(QWidget* parent = 0);

    inline void setColor(const QColor& color) { _color = color; this->update(); }
    inline const QColor& color(void) const { return _color; }
    inline float posSlide(void) const { return _posSlide; }

signals:
    void slideChanged(const float pos);

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void leaveEvent(QEvent* event);

private:
    QColor _color;
    QRect _rectSlider;
    bool _highlighted;
    bool _pressed;
    QPoint _posMouse;
    float _posSlide;
};

#endif

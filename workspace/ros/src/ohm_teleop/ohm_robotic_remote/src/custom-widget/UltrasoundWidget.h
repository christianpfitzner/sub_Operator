#ifndef __ULTRASOUND_WIDGET__
#define __ULTRASOUND_WIDGET__

#include <QWidget>
#include <QVector>
#include <QPixmap>
#include <QRect>

class DistanceWidget;

class UltrasoundWidget : public QWidget
{
    Q_OBJECT;

public:
    UltrasoundWidget(QWidget* parent = 0);

public slots:
    void sensorValues(const QVector<float>& values);

protected:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);

private:
    QPixmap _objectImage;
    QVector<DistanceWidget*> _distances;
    QRect _imagePos;
};

#endif

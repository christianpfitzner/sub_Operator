/*******************************************************************************************/
/* Class HorizontalSlider is a simple slider widget.                                       */
/*                                                                                         */
/* Author: Christian Merkl                                                                 */
/* Date  : 6. June 2013                                                                    */
/*******************************************************************************************/
#ifndef __HORIZONTAL_SLIDER__
#define __HORIZONTAL_SLIDER__

#include <QWidget>
#include <QString>
#include <QColor>

//! horizontal slider class
/*!
 * this is a simple slider class.
 */
class HorizontalSlider : public QWidget
{
    Q_OBJECT

public:
    //! default constructor
    HorizontalSlider(QWidget* parent = 0);

    //! set lower limit.
    void setMinimum(const int min);
    //! set upper limit.
    void setMaximum(const int max);
    //! returns current value.
    int value(void) const { return _value; }
    //! set the widget color.
    void setColor(const QColor& color) { _color = color; this->update(); }

public slots:
    //! set current value.
    void setValue(const int value);
    //! set text that will be shown.
    void setText(const QString& text);

protected:
    //! overwrite
    virtual void paintEvent(QPaintEvent* event);

private:
    //! holds lower limit.
    int _min;
    //! holds upper limit.
    int _max;
    //! holds current value.
    int _value;
    //! holds text that was shown.
    QString _text;
    //! holds widget color.
    QColor _color;
};

#endif

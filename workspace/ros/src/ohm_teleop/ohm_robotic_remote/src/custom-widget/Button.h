/*******************************************************************************************/
/* Class Button is a subclass of AbstractButton. This class can be a right, left and       */
/* Normal button in Star Trek look. Have fun with this...                                  */
/*                                                                                         */
/* Author: Christian Merkl                                                                 */
/* Date  : 2. June 2013                                                                    */
/*******************************************************************************************/
#ifndef __BUTTON__
#define __BUTTON__

#include "AbstractButton.h"

#include <QVector>
#include <QPoint>
#include <QColor>
#include <QPixmap>

//! button in Star Trek look.
/*!
 * is a subclass of AbstractButton. This class provides three looks in Star Trek design,
 * left, right and normal.
 */
class Button : public AbstractButton
{
    Q_OBJECT

public:

    //! button types
    enum Type {
        Normal,
        Right,
        Left,
        Triangle
    };

    //! default constructor.
    explicit Button(QWidget* parent = 0, const QColor& color = Qt::green, const Type type = Normal);

    //! set the button type.
    void setType(const Type type);
    //! set the text that will be printed on the button.
    void setText(const QString& text);
    //! set the color of the button.
    void setColor(const QColor& color);
    //! set icon.
    void setIcon(const QPixmap& icon) { _icon = icon; this->update(); }
    //! returns the color of this button.
    const QColor& color(void) const { return _color; }
    //! returns the current printed button text.
    const QString& text(void) const { return _text; }
    //! returns the icon of this button.
    const QPixmap& icon(void) const { return _icon; }

protected:
    //! overloaded method.
    virtual void paintEvent(QPaintEvent* event);
    //! overloaded method.
    virtual void mouseMoveEvent(QMouseEvent* event);
    //! overloaded method.
    virtual void mousePressEvent(QMouseEvent* event);
    //! overloaded method.
    virtual void mouseReleaseEvent(QMouseEvent* event);
    //! overloaded method.
    virtual void resizeEvent(QResizeEvent* event);
    //! overloaded method.
    virtual void leaveEvent(QEvent* event);

private:

    //! for internal
    enum Point {
        LT = 0,
        RB,
        Center,
        Count
    };

    //! for internal
    QVector<QPoint> _points;
    //! for internal
    bool _mouseIn;
    //! for internal
    QVector<float> _elipsePar;
    //! color of the button
    QColor _color;
    //! holds button type
    Type _type;
    //! holds button text
    QString _text;
    //! holds icon
    QPixmap _icon;
};

#endif

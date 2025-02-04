/*******************************************************************************************/
/* Class AbstractSpinBox is a abstract base class for spin boxes. This class contains      */
/* the view part.                                                                          */
/*                                                                                         */
/* Author: Christian Merkl                                                                 */
/* Date  : 1. June 2013                                                                    */
/*******************************************************************************************/
#ifndef __ABSTRACT_SPIN_BOX__
#define __ABSTRACT_SPIN_BOX__

#include <QWidget>
#include <QColor>

/* include will be generated from ui file */
#include "ui_AbstractSpinBox.h"

//! abstract class AbstractSpinBox
/*!
  Class AbstractSpinBox is a abstract base class for spin boxes. This class contains the view part.
*/
class AbstractSpinBox : public QWidget
{
    Q_OBJECT

public:
    //! constructor.
    explicit AbstractSpinBox(QWidget* parent = 0, const QColor& color = QColor(Qt::red));

    //! set the color of this widget.
    void setColor(const QColor& color);
    //! returns the color of this widget
    const QColor& color(void) const { return _color; }

public slots:
    //! increments the spin box.
    /*!
     * is a abstract method and must be implemented in subclasses. This slot will be connected with
     * the clicked() signal of the right button by constructor.
     */
    virtual void increment(void) = 0;

    //! decrements the spin box.
    /*!
     * is a abstract method and must be implemented in subclasses. This slot will be connected with
     * the clicked() signal of the left button by constructor.
     */
    virtual void decrement(void) = 0;

protected:
    //! contains all gui elements.
    Ui::AbstractSpinBox* _ui;

private:
    //! holds the color of this widget.
    QColor _color;
};

#endif

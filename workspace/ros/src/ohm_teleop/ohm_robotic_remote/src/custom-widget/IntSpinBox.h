/*******************************************************************************************/
/* Class IntSpinBox is a subclass of AbstractSpinBox with int number model.                */
/*                                                                                         */
/* Author: Christian Merkl                                                                 */
/* Date  : 1. June 2013                                                                    */
/*******************************************************************************************/
#ifndef __INT_SPIN_BOX__
#define __INT_SPIN_BOX__

#include "AbstractSpinBox.h"

//! Int Spin Box
/*!
 * is a subclass of AbstractSpinBox with int number model.
 */
class IntSpinBox : public AbstractSpinBox
{
    Q_OBJECT

public:
    //! default constructor
    explicit IntSpinBox(QWidget* parent = 0, const QColor& color = QColor(Qt::red));

    //! sets the lower limit of this spin box.
    /*!
     * if current value < min then it will be changed to min.
     */
    void setMin(const int min);

    //! sets the upper limit of this spin box.
    /*!
     * if current value > max then it will be changed to max.
     */
    void setMax(const int max);

    //! returns current lower limit.
    int min(void) const { return _min; }
    //! returns current upper limit.
    int max(void) const { return _max; }
    //! returns current value.
    int value(void) const { return _value; }

public slots:
    //! sets the current value
    void setValue(const int value);
    //! increments current value by 1.
    /*!
     * if the current value reaches the upper limit nothing will happens.
     */
    virtual void increment(void);
    //! decrements current value by 1.
    /*!
     * if the current value reaches the lower limit nothing will happens.
     */
    virtual void decrement(void);

signals:
    //! signal emits when the current value was changed.
    void valueChanged(const int newValue);

private:

    //! lower limit
    int _min;
    //! upper limit
    int _max;
    //! current value
    int _value;
};

#endif

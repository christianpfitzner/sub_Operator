/*******************************************************************************************/
/* Class AbstractButton is a abstract base class for buttons and provides basic mehtods    */
/* for them.                                                                               */
/*                                                                                         */
/* Author: Christian Merkl                                                                 */
/* Date  : 2. June 2013                                                                    */
/*******************************************************************************************/
#ifndef __ABSTRACT_BUTTON__
#define __ABSTRACT_BUTTON__

#include <QWidget>

//! abstract base class for buttons.
class AbstractButton : public QWidget
{
    Q_OBJECT

public:
    //! default constructor
    explicit AbstractButton(QWidget* parent = 0)
        : QWidget(parent), _pressed(false), _clickable(true), _checkable(false) { }

    //! returns true if the button is pressed.
    bool isPressed(void) const { return _pressed; }
    //! set this button clickable.
    /*!
     * if the button is not clickable no mouse event will be received by the button.
     */
    void setClickable(const bool enable = true) { _clickable = enable; }
    //! returns true if the button is clickable.
    bool isClickable(void) const { return _clickable; }
    //! set this button as pressed.
    /*!
     * is the same as the button was pressed by mouse.
     */
    void setPressed(const bool pressed = true) { _pressed = pressed; this->update(); }
    //! set button checkable.
    void setCheckable(const bool checkable = true) { _checkable = checkable; }
    //! returns if the button is checkable or not.
    bool isCheckable(void) const { return _checkable; }

signals:
    //! signal will be emitted when button was pressed.
    void pressed(void);
    //! signal will be emitted when button was released.
    void released(void);
    //! signal will be emitted when button was clicked.
    void clicked(void);

protected:
    //! holds if button is pressed.
    bool _pressed;
    //! holds if button is clickable.
    bool _clickable;
    //! holds if button
    bool _checkable;
};

#endif

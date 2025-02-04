/*******************************************************************************************/
/* Class Co2Widget shows co2 level.                                                        */
/*                                                                                         */
/* Author: Christian Merkl                                                                 */
/* Date  : 6. June 2013                                                                    */
/*******************************************************************************************/
#ifndef __CO2_WIDGET__
#define __CO2_WIDGET__

#include <QWidget>

namespace Ui {
class Co2Widget;
}

class QColor;

//! class to show co2 level.
class Co2Widget : public QWidget
{
public:
    //! default constructor
    Co2Widget(QWidget* parent = 0);

    //! set the widget color.
    void setColor(const QColor& color);
    //! set the current co2 value.
    void setCo2Value(const int value);

private:
    //! holds ui components.
    Ui::Co2Widget* _ui;
};

#endif

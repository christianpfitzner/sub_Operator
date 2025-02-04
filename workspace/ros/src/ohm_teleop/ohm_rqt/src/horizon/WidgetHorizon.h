/**************************************************************\
 * Authors: Yannick Wallerer
 *          Rainer Walther
 *          Philipp Weiß
 *
 * Date:    11.02.2019
 *
 * Description: The Widget Contains two variables representing
 *              the two angles pitch and roll in RADIANT.
 *
 *              The following value ranges can be displayed:
 *
 *              Value range pitch: -90°  <= pitch <= 90°
 *                                 -PI/2 <= pitch <= PI/2
 *
 *              Value range roll:  -45°  <= roll  <= 45°
 *                                 -PI/4 <= roll  <= PI/4
\**************************************************************/

#include <QtWidgets/QWidget>

#define PI   3.141592654
#define PI_2 1.570796327 // PI / 2
#define PI_4 0.785398163 // PI / 4

class WidgetHorizon : public QWidget
{
private:
  float pitch, roll; // angles pitch and roll

public:
  WidgetHorizon(void);
  ~WidgetHorizon(void);
  void setPitch(float p);
  void setRoll(float r);

protected:
  void paintEvent(QPaintEvent* event);
  void paintHorizon(float pitch, float roll);
  void paintMask(void);
  void paintCross(float pitch, float roll);
  void paintScale(void);
};

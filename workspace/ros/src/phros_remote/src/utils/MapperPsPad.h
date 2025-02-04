#ifndef MAPPERPSPAD_H_
#define MAPPERPSPAD_H_

#include "StateButton.h"
#include "mappers/PsProfiles.h"
#include <memory>
#include <sensor_msgs/Joy.h>

namespace phros_remote
{

namespace
{
const uint8_t     N_BUTTONS_PS = 17;
const uint8_t     N_AXES       = 6;
const std::string typePs3      = "ps_3";
const std::string typePs4      = "ps_4";
} // namespace

class MapperPsPad
{
public:
  enum class ButtonsPad
  {
    X = 0,
    C,
    T,
    S,
    L1,
    R1,
    L2,
    R2,
    SL,
    ST,
    PS,
    A1,
    A2,
    CROSS_LEFT,
    CROSS_RIGHT,
    CROSS_UP,
    CROSS_DOWN
  };
  enum class AxesPad
  {
    STICK_LEFT_X = 0,
    STICK_LEFT_Y,
    STICK_RIGHT_X,
    STICK_RIGHT_Y,
    L2,
    R2,
    LAST
  };
  MapperPsPad();
  virtual ~MapperPsPad() {}
  virtual void      map(const sensor_msgs::Joy& joy) = 0;
  virtual void      print()                          = 0;
  const StateButton button(const ButtonsPad& button) const { return _buttons[static_cast<unsigned int>(button)]; }
  const float       axis(const AxesPad& axis) const { return _axes[static_cast<unsigned int>(axis)]; }
  const StateButton button(const int index) const { return _buttons[index];}
  const float       axis(const int index) const { return _axes[index];}
  
  void              reset();
  // bool              buttonPressedX(void) const { return _buttonPressedX; }
  // bool              buttonPressedC(void) const { return _buttonPressedC; }
  // bool              buttonPressedT(void) const { return _buttonPressedT; }
  // bool              buttonPressedS(void) const { return _buttonPressedS; }
  // bool              buttonPressedL1(void) const { return _buttonPressedL1; }
  // bool              buttonPressedR1(void) const { return _buttonPressedR1; }
  // bool              buttonPressedL2(void) const { return _buttonPressedL2; }
  // bool              buttonPressedR2(void) const { return _buttonPressedR2; }
  // bool              buttonPressedSL(void) const { return _buttonPressedSL; }
  // bool              buttonPressedST(void) const { return _buttonPressedST; }
  // bool              buttonPressedPS(void) const { return _buttonPressedPS; }
  // bool              buttonPressedA1(void) const { return _buttonPressedA1; }
  // bool              buttonPressedA2(void) const { return _buttonPressedA2; }
  // bool              crossLeft(void) const { return _crossLeft; }
  // bool              crossRight(void) const { return _crossRight; }
  // bool              crossUp(void) const { return _crossUp; }
  // bool              crossDown(void) const { return _crossDown; }
  // const float       stickLeftX(void) const { return _stickLeftX; }
  // const float       stickLeftY(void) const { return _stickLeftY; }
  // const float       stickRightX(void) const { return _stickRightX; }
  // const float       stickRightY(void) const { return _stickRightY; }
  // const float       L2(void) const { return _L2; }
  // const float       R2(void) const { return _R2; }

protected:
  std::vector<StateButton> _buttons;
  std::vector<float>       _axes;
  // bool                     _buttonPressedX  = false;
  // bool                     _buttonPressedC  = false;
  // bool                     _buttonPressedT  = false;
  // bool                     _buttonPressedS  = false;
  // bool                     _buttonPressedL1 = false;
  // bool                     _buttonPressedR1 = false;
  // bool                     _buttonPressedL2 = false;
  // bool                     _buttonPressedR2 = false;
  // bool                     _buttonPressedSL = false;
  // bool                     _buttonPressedST = false;
  // bool                     _buttonPressedPS = false;
  // bool                     _buttonPressedA1 = false;
  // bool                     _buttonPressedA2 = false;
  // bool                     _crossLeft       = false;
  // bool                     _crossRight      = false;
  // bool                     _crossUp         = false;
  // bool                     _crossDown       = false;
  // float                    _stickLeftX      = 0.0f;
  // float                    _stickLeftY      = 0.0f;
  // float                    _stickRightX     = 0.0f;
  // float                    _stickRightY     = 0.0f;
  // float                    _L2              = 0.0f;
  // float                    _R2              = 0.0f;
};

} // namespace phros_remote
#endif
#include "MapperPs4Pad.h"

namespace phros_remote
{

void MapperPs4Pad::map(const sensor_msgs::Joy& joy)
{
  _buttons[static_cast<unsigned int>(ButtonsPad::X)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs4::B_X)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::C)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs4::B_C)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::T)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs4::B_T)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::S)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs4::B_S)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::L1)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs4::B_L1)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::R1)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs4::B_R1)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::L2)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs4::B_L2)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::R2)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs4::B_R2)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::SL)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs4::B_SH)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::ST)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs4::B_OPT)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::PS)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs4::B_PS)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::A1)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs4::B_A1)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::A2)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs4::B_A2)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::CROSS_LEFT)].update(joy.axes[static_cast<unsigned int>(AxesPs4::X_X)] > AXE_MAX);
  _buttons[static_cast<unsigned int>(ButtonsPad::CROSS_RIGHT)].update(joy.axes[static_cast<unsigned int>(AxesPs4::X_X)] < -1.0f * AXE_MAX);
  _buttons[static_cast<unsigned int>(ButtonsPad::CROSS_UP)].update(joy.axes[static_cast<unsigned int>(AxesPs4::X_Y)] > AXE_MAX);
  _buttons[static_cast<unsigned int>(ButtonsPad::CROSS_DOWN)].update(joy.axes[static_cast<unsigned int>(AxesPs4::X_Y)] < -1.0f * AXE_MAX);

  _axes[static_cast<unsigned int>(AxesPad::STICK_LEFT_X)]  = joy.axes[static_cast<unsigned int>(AxesPs4::A1_X)];
  _axes[static_cast<unsigned int>(AxesPad::STICK_LEFT_Y)]  = joy.axes[static_cast<unsigned int>(AxesPs4::A1_Y)];
  _axes[static_cast<unsigned int>(AxesPad::STICK_RIGHT_X)] = joy.axes[static_cast<unsigned int>(AxesPs4::A2_X)];
  _axes[static_cast<unsigned int>(AxesPad::STICK_RIGHT_Y)] = joy.axes[static_cast<unsigned int>(AxesPs4::A2_Y)];
  _axes[static_cast<unsigned int>(AxesPad::L2)]            = joy.axes[static_cast<unsigned int>(AxesPs4::L2)];
  _axes[static_cast<unsigned int>(AxesPad::R2)]            = joy.axes[static_cast<unsigned int>(AxesPs4::R2)];
}

void MapperPs4Pad::print()
{
  std::cout << __PRETTY_FUNCTION__ << " buttons (state/edge): ";
  for(auto& iter : _buttons)
    iter.print();
  std::cout << __PRETTY_FUNCTION__ << " axes: ";
  for(auto& iter : _axes)
    std::cout << iter << " ";
  std::cout << std::endl;
}

} // namespace phros_remote
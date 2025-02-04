#include "MapperPs3Pad.h"

namespace phros_remote
{

void MapperPs3Pad::map(const sensor_msgs::Joy &joy)
{

  _buttons[static_cast<unsigned int>(ButtonsPad::X)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs3::B_X)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::C)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs3::B_C)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::T)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs3::B_T)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::S)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs3::B_S)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::L1)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs3::B_L1)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::R1)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs3::B_R1)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::L2)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs3::B_L2)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::R2)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs3::B_R2)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::SL)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs3::B_SELECT)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::ST)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs3::B_START)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::PS)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs3::B_PS)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::A1)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs3::B_A1)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::A2)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs3::B_A2)]);
  
  _buttons[static_cast<unsigned int>(ButtonsPad::CROSS_LEFT)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs3::B_LEFT)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::CROSS_RIGHT)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs3::B_RIGHT)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::CROSS_UP)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs3::B_UP)]);
  _buttons[static_cast<unsigned int>(ButtonsPad::CROSS_DOWN)].update(joy.buttons[static_cast<unsigned int>(ButtonsPs3::B_DOWN)]);

  _axes[static_cast<unsigned int>(AxesPad::STICK_LEFT_X)]  = joy.axes[static_cast<unsigned int>(AxesPs3::A1_X)];
  _axes[static_cast<unsigned int>(AxesPad::STICK_LEFT_Y)]  = joy.axes[static_cast<unsigned int>(AxesPs3::A1_Y)];
  _axes[static_cast<unsigned int>(AxesPad::STICK_RIGHT_X)] = joy.axes[static_cast<unsigned int>(AxesPs3::A2_X)];
  _axes[static_cast<unsigned int>(AxesPad::STICK_RIGHT_Y)] = joy.axes[static_cast<unsigned int>(AxesPs3::A2_Y)];
  _axes[static_cast<unsigned int>(AxesPad::L2)]            = joy.axes[static_cast<unsigned int>(AxesPs3::L2)];
  _axes[static_cast<unsigned int>(AxesPad::R2)]            = joy.axes[static_cast<unsigned int>(AxesPs3::R2)];
}

void MapperPs3Pad::print()
{
  std::cout << __PRETTY_FUNCTION__ << " buttons (state/edge): ";
  for(auto& iter : _buttons)
    iter.print();
  std::cout << __PRETTY_FUNCTION__ << " axes: ";
  for(auto& iter : _axes)
    std::cout << iter << " ";
  std::cout << std::endl;
}

}
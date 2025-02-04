/*
 * LocalizationFlipper.h
 *
 *  Created on: Apr 13, 2018
 *      Author: phil
 */

#ifndef OHM_SCHROEDI_MC_SRC_LOCALIZATIONFLIPPER_H_
#define OHM_SCHROEDI_MC_SRC_LOCALIZATIONFLIPPER_H_

#include <string>
#include <tf/transform_broadcaster.h>

namespace
{
static const double GEAR_R = 6.75 * 216.0;
static const double TICS_P_TURN = 512.0;
}

class LocalizationFlipper
{
public:
  LocalizationFlipper(const std::string& base, ros::NodeHandle& nh);
  virtual ~LocalizationFlipper();
  void update(const int32_t& ticksPresent);
private:
  double flipperTicksToAngle(const int32_t& ticks);
  ros::NodeHandle& _nh;
  ros::Publisher _pubAnglePresent;
  std::string _frameBase;
  std::string _frameChild;
};

#endif /* OHM_SCHROEDI_MC_SRC_LOCALIZATIONFLIPPER_H_ */

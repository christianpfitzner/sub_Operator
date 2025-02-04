/*
 * MapperController.h
 *
 *  Created on: May 8, 2019
 *      Author: phil
 */

#ifndef OHM_RQT_SRC_REMOTE_MAPPERCONTROLLER_H_
#define OHM_RQT_SRC_REMOTE_MAPPERCONTROLLER_H_

#include <sensor_msgs/Joy.h>
#include "mappers/IMapper.h"
#include <memory>
#include <std_msgs/String.h>

namespace phros_remote
{
class MapperPsPad;
class MapperController
{
public:
  // enum class AxisActions
  // {
  //   MOVELEFTRIGHT = 0,
  //   SHPITCH,    //sh -> Sensorhead
  //   SHYAW,
  //   ARMFWBW,
  //   ARMLEFTRIGHT,
  //   ARMUPDOWN
  // };
  // enum class ButtonsActions
  // {
  //   MAPPERSWITCH = 0,
  //   MOVEFWD,
  //   MOVEBWD,
  //   SHHOME,
  //   FLIPPERCOMPACT,
  //   FLIPPERW,
  //   FLIPPERUP,
  //   FLIPPERDOWN,
  //   FLIPPERFL,
  //   FLIPPERFR,
  //   FLIPPERBL,
  //   FLIPPERBR,
  //   DRIVESWITCH1,
  //   DRIVESWITCH2,
  //   OPENGRP,
  //   CLOSEGRP,
  //   ROTGRPL,
  //   ROTGRPR,
  //   ROTWRSTL,
  //   ROSWRSTR,
  //   ROTWRSTU,
  //   ROTWRSTD,
  //   LOOKDOWN,
  //   HOMEPOS,
  //   FOLDARM,
  //   CHANGECORD
  // };

  class AxisActions
  {
    public:
    int moveLeftRight = 0;
    int moveFwd = 5;
    int moveBwd = 4;
    int shPitch = 3;    //Sh -> Sensor head
    int shYaw = 2;
    int armFwBw = 1;
    int armLeftRight = 0;
    int armUpDown = 3;
  };

  class ButtonActions
  {
    public:
    int mapperSwitch = 10;
    int shHome = 12;
    int flipperCompact = 14;
    int flipperW = 13;
    int flipperUp = 15;
    int flipperDown = 16;
    int flipperFl = 2;
    int flipperFr = 1;
    int flipperBl = 3;
    int flipperBr = 0;
    int driveSwitch1 = 5;
    int driveSwitch2 = 4;
    int openGrp = 3; //open gripper
    int closeGrp = 1;
    int rotGrpL = 6; //rotat gripper left
    int rotGrpR = 7;
    int rotWrstL = 4; //rotate Wrist left
    int rotWrstR = 5;
    int rotWrstU = 2;
    int rotWrstD = 0;
    int lookDown = 16;
    int homePos = 15;
    int foldArm = 9;
    int changeCord = 8;
  };

  static std::shared_ptr<MapperController> getInstance(void);
  virtual ~MapperController();
  void map(std::shared_ptr<MapperPsPad>& msg);
  void switchConfig(std::shared_ptr<std_msgs::String>& msg);
  void mapImage(void){_mapper->mapImage();}
  bool switchMapper(const IMapper::RemoteType& type, const bool switched = false);
  bool switchMapper(void);
  const IMapper::RemoteType type(void)const{return _mapper->type();}
private:
  MapperController(void);
  ros::NodeHandle _nh;
  std::shared_ptr<IMapper> _mapper;
  std::vector<std::shared_ptr<IMapper> > _mappers;
  static std::shared_ptr<MapperController> _instance;
  IMapper::RemoteType _last;

  ButtonActions btnActions;
  AxisActions axisActions;
};

} /* namespace ohm_remote */

#endif /* OHM_RQT_SRC_REMOTE_MAPPERCONTROLLER_H_ */

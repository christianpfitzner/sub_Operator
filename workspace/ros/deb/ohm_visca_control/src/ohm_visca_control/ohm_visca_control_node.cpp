/*
 * ohm_visca_control_node.cpp.cpp
 *
 *  Created on: 07.12.2018
 *      Author: volletjo
 */
#include <ros/ros.h>
#include "ViscaSerial.h"

#include <dynamic_reconfigure/server.h>
#include <ohm_visca_control/ohm_visca_controlConfig.h>
//#include <ohm_visca_control/viscaControl.h>
#include "ViscaSend.h"
#include "std_msgs/UInt16.h"
ohm_visca_control::ohm_visca_controlConfig _config;

VISCAInterface_t iface;
VISCACamera_t camera;
const char *device_name = "/dev/tamron-zoom-control";

bool new_value = 0;
void callback(ohm_visca_control::ohm_visca_controlConfig &config, uint32_t level) {
	ROS_INFO("Reconfigure Request: %i,%s,%i,%s,%d,%s,%d, %i,%i,%d,%s,%d,%d,%s,%d,%s,%s,%i,%i,%i,%s,%d,%i,%i,%i,%d,%s,%s,%s,%s,%d,%i,%i",
			config.zoom_direct,
			config.autofocus?"True":"False",
			config.focus_direct,
			config.autofocus_sensitivity?"True":"False",
	    config.af_mode,
	    config.vibration_compensation?"True":"False",
	    config.wb_mode,
	    config.rgain_direct,
	    config.bgain_direct,
	    config.ae_mode,
      config.slow_shutter?"True":"False",
	    config.shutter_manual,
	    config.iris_manual,
	    config.exp_comp?"True":"False",
	    config.exp_comp_manual,
	    config.backlight_compensation?"True":"False",
	    config.spotAE?"True":"False",
	    config.spotAE_position_x,
	    config.spotAE_position_y,
	    config.AE_response,
	    config.wide_dynamic_range?"True":"False",
      config.defog,
      config.aperture_direct,
      config.value3DNR,
      config.value2DNR,
      config.gamma,
      config.lr_reverse?"True":"False",
      config.freeze?"True":"False",
      config.picture_effect?"True":"False",
      config.picture_flip?"True":"False",
      config.chroma_suppression,
      config.color_gain,
      config.color_hue);
	_config = config;
new_value = 1;
}


void viscaControlCallback(const ohm_visca_control::viscaControl::ConstPtr& control_msg)
{
  ohm_visca_control::viscaControl control_data;
  control_data.teleStandard = control_msg->teleStandard;
  control_data.wideStandard = control_msg->wideStandard;
  control_data.teleVariable = control_msg->teleVariable;
  control_data.wideVariable = control_msg->wideVariable;
  control_data.variableSpeed = control_msg->variableSpeed;
  control_data.zoomDirect = control_msg->zoomDirect;
  control_data.autofocus = control_msg->autofocus;
  control_data.farStandard = control_msg->farStandard;
  control_data.nearStandard = control_msg->nearStandard;
  control_data.farVariable = control_msg->farVariable;
  control_data.nearVariable = control_msg->nearVariable;
  control_data.variableLevel = control_msg->variableLevel;
  control_data.focusDirect = control_msg->focusDirect;
  control_data.zoomFocusDirect = control_msg->zoomFocusDirect;
  viscaSendControl.send_control_data(&iface, &camera, control_data);
}

int main(int argc, char **argv)
{

 ros::init(argc, argv, "ohm_visca_control_node");
 ros::NodeHandle n;
 dynamic_reconfigure::Server<ohm_visca_control::ohm_visca_controlConfig> server;
 dynamic_reconfigure::Server<ohm_visca_control::ohm_visca_controlConfig>::CallbackType f;

  f = boost::bind(&callback, _1, _2);
  server.setCallback(f);
  ros::Publisher zoom_pub = n.advertise<std_msgs::UInt16>("camera_zoom", 1000);
//  ViscaClass viscaSerial;

  ros::Subscriber viscaControlSub = n.subscribe("viscaControl", 1, viscaControlCallback);


  if (viscaSerial.open_serial(&iface, device_name)!=VISCA_SUCCESS)
    {
      fprintf(stderr,"unable to open serial device %s\n",device_name);
      exit(1);
    }

  int camera_num;
  iface.broadcast=0;

  viscaSerial.set_address(&iface, &camera_num);

  camera.address=1;
  viscaSerial.clear(&iface, &camera);
  viscaSerial.get_version(&iface, &camera);
  fprintf(stderr,"Some camera info:\n------------------\n");
  fprintf(stderr,"vendor: 0x%04x\n model: 0x%04x\n ROM version: 0x%04x\n socket number: 0x%02x\n",
	  camera.vendor, camera.model, camera.rom_version, camera.socket_num);
  viscaSerial.usleep(500000);

  //int32_t CAM_Zoom_Direct_Value =config.CAM_Zoom_Direct_Value;
  ros::Rate rate(30);
  while (ros::ok())
  {
	  if( new_value == 1)
	  {
  //std::cout<< _config.set_zoom_direct << std::endl;
	    viscaSerial.set_digital_zoom_power(&iface, &camera,CAM_Dzoom_On);
  viscaSerial.set_zoom_direct(&iface, &camera,(uint32_t)(_config.zoom_direct));




  if(_config.autofocus == 1)
  {
    std::cout << "autofocus ON" << std::endl;
    viscaSerial.set_focus_auto(&iface, &camera);
    if (_config.autofocus_sensitivity == 1)
    {
      viscaSerial.set_focus_autosense_low(&iface, &camera);
    }
    else
    {
      viscaSerial.set_focus_autosense_normal(&iface, &camera);
    }
    uint8_t af_mode = CAM_AFMode_Normal;
    switch (_config.af_mode)
    {
    case 0: af_mode=CAM_AFMode_Normal;
    break;
    case 1: af_mode=CAM_AFMode_Interval;
    break;
    case 2: af_mode=CAM_AFMode_Zoom_Trigger;
    break;
    default: af_mode=CAM_AFMode_Normal;
    }
    viscaSerial.set_afmode(&iface, &camera,af_mode);
  }
  else
  {
    std::cout << "autofocus OFF" << std::endl;
    viscaSerial.set_focus_manual(&iface, &camera);
    viscaSerial.set_focus_direct(&iface, &camera,(uint32_t)(_config.focus_direct));
  }
  if (_config.vibration_compensation == 1)
  {
    viscaSerial.set_vibration_compensation_power(&iface, &camera,CAM_VibrationCompensation_On);
    std::cout << "vibration compensation ON" << std::endl;
  }
  else
  {
    viscaSerial.set_vibration_compensation_power(&iface, &camera,CAM_VibrationCompensation_Off);
    std::cout << "vibration compensation OFF" << std::endl;
  }
  uint8_t wb_mode = CAM_WB_ATW_Narrow;
  switch (_config.wb_mode)
  {
  case 0: wb_mode=CAM_WB_ATW_Narrow;
  break;
  case 1: wb_mode=CAM_WB_Indoor;
  break;
  case 2: wb_mode=CAM_WB_Outdoor;
  break;
  case 3: wb_mode=CAM_WB_One_Push_WB;
  break;
  case 4: wb_mode=CAM_WB_ATW_Wide;
  break;
  case 5: wb_mode=CAM_WB_Manual;
  break;
  default: wb_mode=CAM_WB_ATW_Narrow;
  }
  viscaSerial.set_whitebalance_mode(&iface, &camera,(uint8_t)(wb_mode));
  if(_config.wb_mode == 5)
  {
    std::cout << "Setting manual gain" << std::endl;
    viscaSerial.set_rgain_direct(&iface, &camera, (uint8_t)(_config.rgain_direct));
    viscaSerial.usleep(200);
    viscaSerial.set_bgain_direct(&iface, &camera, (uint8_t)(_config.bgain_direct));
  }
  bool shutter_manual_mode = 0;
  bool iris_manual_mode = 0;
  bool gain_manual_mode = 0;
  bool exp_comp_avail = 0;
  uint8_t ae_mode = CAM_AE_Full_Auto;
  switch (_config.ae_mode)
  {
  case 0: ae_mode=CAM_AE_Full_Auto;
  shutter_manual_mode = 0;
  iris_manual_mode = 0;
  gain_manual_mode = 0;
  exp_comp_avail = 1;
  break;
  case 1: ae_mode=CAM_AE_Manual;
  shutter_manual_mode = 1;
  iris_manual_mode = 1;
  gain_manual_mode = 1;
  exp_comp_avail = 0;
  break;
  case 2: ae_mode=CAM_AE_Shutter_Priority;
  shutter_manual_mode = 1;
  iris_manual_mode = 0;
  gain_manual_mode = 0;
  exp_comp_avail = 1;
  break;
  case 3: ae_mode=CAM_AE_Iris_Priority;
  shutter_manual_mode = 0;
  iris_manual_mode = 1;
  gain_manual_mode = 0;
  exp_comp_avail = 1;
  break;
  default: ae_mode=CAM_AE_Full_Auto;
  shutter_manual_mode = 0;
  iris_manual_mode = 0;
  gain_manual_mode = 0;
  exp_comp_avail = 1;
  }
  viscaSerial.set_auto_exp_mode(&iface, &camera,(uint8_t)(ae_mode));
  if(_config.slow_shutter == 1)
  {
    viscaSerial.set_slow_shutter_auto(&iface, &camera,(uint8_t)(CAM_SlowShutter_Auto));
  }
  else
  {
    viscaSerial.set_slow_shutter_auto(&iface, &camera,(uint8_t)(CAM_SlowShutter_Manual));
  }

  if (shutter_manual_mode == 1)
  {
  uint8_t shutter_manual = CAM_Shutter_Direct_1_30;
  switch (_config.shutter_manual)
  {
  case 0: shutter_manual=CAM_Shutter_Direct_1_1;
  break;
  case 1: shutter_manual=CAM_Shutter_Direct_1_2;
  break;
  case 2: shutter_manual=CAM_Shutter_Direct_1_4;
  break;
  case 3: shutter_manual=CAM_Shutter_Direct_1_8;
  break;
  case 4: shutter_manual=CAM_Shutter_Direct_1_15;
  break;
  case 5: shutter_manual=CAM_Shutter_Direct_1_30;
  break;
  case 6: shutter_manual=CAM_Shutter_Direct_1_60;
  break;
  case 7: shutter_manual=CAM_Shutter_Direct_1_90;
  break;
  case 8: shutter_manual=CAM_Shutter_Direct_1_100;
  break;
  case 9: shutter_manual=CAM_Shutter_Direct_1_120;
  break;
  case 10: shutter_manual=CAM_Shutter_Direct_1_180;
  break;
  case 11: shutter_manual=CAM_Shutter_Direct_1_250;
  break;
  case 12: shutter_manual=CAM_Shutter_Direct_1_350;
  break;
  case 13: shutter_manual=CAM_Shutter_Direct_1_500;
  break;
  case 14: shutter_manual=CAM_Shutter_Direct_1_725;
  break;
  case 15: shutter_manual=CAM_Shutter_Direct_1_1000;
  break;
  case 16: shutter_manual=CAM_Shutter_Direct_1_1500;
  break;
  case 17: shutter_manual=CAM_Shutter_Direct_1_2000;
  break;
  case 18: shutter_manual=CAM_Shutter_Direct_1_3000;
  break;
  case 19: shutter_manual=CAM_Shutter_Direct_1_4000;
  break;
  case 20: shutter_manual=CAM_Shutter_Direct_1_6000;
  break;
  case 21: shutter_manual=CAM_Shutter_Direct_1_10000;
  break;
  default: shutter_manual=CAM_Shutter_Direct_1_30;
  }
  std::cout << "Shutter manual:"<< (double)(shutter_manual) << std::endl;
viscaSerial.set_shutter_direct(&iface, &camera,(uint8_t)(shutter_manual));
  }
  if(iris_manual_mode == 1)
  {
    uint8_t iris_manual = CAM_Iris_Direct_F4_8;
    switch (_config.iris_manual)
    {
    case 0: iris_manual=CAM_Iris_Direct_Close ;
    break;
    case 1: iris_manual=CAM_Iris_Direct_F22_0;
    break;
    case 2: iris_manual=CAM_Iris_Direct_F16_0;
    break;
    case 3: iris_manual=CAM_Iris_Direct_F14_0;
    break;
    case 4: iris_manual=CAM_Iris_Direct_F11_0;
    break;
    case 5: iris_manual=CAM_Iris_Direct_F9_6;
    break;
    case 6: iris_manual=CAM_Iris_Direct_F8_0;
    break;
    case 7: iris_manual=CAM_Iris_Direct_F6_8;
    break;
    case 8: iris_manual=CAM_Iris_Direct_F5_6;
    break;
    case 9: iris_manual=CAM_Iris_Direct_F4_8;
    break;
    case 10: iris_manual=CAM_Iris_Direct_F4_0;
    break;
    case 11: iris_manual=CAM_Iris_Direct_F3_4;
    break;
    case 12: iris_manual=CAM_Iris_Direct_F2_8;
    break;
    case 13: iris_manual=CAM_Iris_Direct_F2_4;
    break;
    case 14: iris_manual=CAM_Iris_Direct_F2_0;
    break;
    case 15: iris_manual=CAM_Iris_Direct_F1_8;
    break;
    default: iris_manual=CAM_Iris_Direct_F4_8;
    }
    viscaSerial.set_iris_direct(&iface, &camera,(uint8_t)(iris_manual));
  }
  if(gain_manual_mode == 1)
  {
    uint8_t gain_manual = CAM_Gain_Direct_p0dB;
    switch (_config.gain_manual)
    {
    case 0: gain_manual=CAM_Gain_Direct_0dB ;
    break;
    case 1: gain_manual=CAM_Gain_Direct_p0dB;
    break;
    case 2: gain_manual=CAM_Gain_Direct_p2dB;
    break;
    case 3: gain_manual=CAM_Gain_Direct_p4dB;
    break;
    case 4: gain_manual=CAM_Gain_Direct_p6dB;
    break;
    case 5: gain_manual=CAM_Gain_Direct_p8dB;
    break;
    case 6: gain_manual=CAM_Gain_Direct_p10dB;
    break;
    case 7: gain_manual=CAM_Gain_Direct_p12dB;
    break;
    case 8: gain_manual=CAM_Gain_Direct_p14dB;
    break;
    case 9: gain_manual=CAM_Gain_Direct_p16dB;
    break;
    case 10: gain_manual=CAM_Gain_Direct_p18dB;
    break;
    case 11: gain_manual=CAM_Gain_Direct_p20dB;
    break;
    case 12: gain_manual=CAM_Gain_Direct_p22dB;
    break;
    case 13: gain_manual=CAM_Gain_Direct_p24dB;
    break;
    case 14: gain_manual=CAM_Gain_Direct_p26dB;
    break;
    case 15: gain_manual=CAM_Gain_Direct_p28dB;
    break;
    case 16: gain_manual=CAM_Gain_Direct_p30dB;
    break;
    case 17: gain_manual=CAM_Gain_Direct_p32dB;
    break;
    case 18: gain_manual=CAM_Gain_Direct_p34dB;
    break;
    case 19: gain_manual=CAM_Gain_Direct_p36dB;
    break;
    case 20: gain_manual=CAM_Gain_Direct_p38dB;
    break;
    case 21: gain_manual=CAM_Gain_Direct_p40dB;
    break;
    case 22: gain_manual=CAM_Gain_Direct_p42dB;
    break;
    case 23: gain_manual=CAM_Gain_Direct_p44dB;
    break;
    case 24: gain_manual=CAM_Gain_Direct_p46dB;
    break;
    default: gain_manual=CAM_Gain_Direct_p0dB;
    }
viscaSerial.set_gain_direct(&iface, &camera,(uint8_t)(gain_manual));
  }
  if(_config.exp_comp == 1)
  {
    viscaSerial.set_exp_comp_power(&iface, &camera,CAM_ExpComp_Enable_On);
  }
  else
  {
    viscaSerial.set_exp_comp_power(&iface, &camera,CAM_ExpComp_Enable_Off);
  }
  if(exp_comp_avail == 1)
  {
    uint8_t exp_comp_manual = CAM_ExpComp_Direct_0dB;
    switch (_config.exp_comp_manual)
    {
    case 0: exp_comp_manual=CAM_ExpComp_Direct_m12dB;
    break;
    case 1: exp_comp_manual=CAM_ExpComp_Direct_m10dB;
    break;
    case 2: exp_comp_manual=CAM_ExpComp_Direct_m8dB;
    break;
    case 3: exp_comp_manual=CAM_ExpComp_Direct_m6dB;
    break;
    case 4: exp_comp_manual=CAM_ExpComp_Direct_m4dB;
    break;
    case 5: exp_comp_manual=CAM_ExpComp_Direct_m2dB;
    break;
    case 6: exp_comp_manual=CAM_ExpComp_Direct_0dB;
    break;
    case 7: exp_comp_manual=CAM_ExpComp_Direct_p2dB;
    break;
    case 8: exp_comp_manual=CAM_ExpComp_Direct_p4dB;
    break;
    case 9: exp_comp_manual=CAM_ExpComp_Direct_p6dB;
    break;
    case 10: exp_comp_manual=CAM_ExpComp_Direct_p8dB;
    break;
    case 11: exp_comp_manual=CAM_ExpComp_Direct_p10dB;
    break;
    case 12: exp_comp_manual=CAM_ExpComp_Direct_p12dB;
    break;
    default: exp_comp_manual=CAM_ExpComp_Direct_0dB;
    }
    viscaSerial.set_exp_comp_direct(&iface, &camera,(uint8_t)(exp_comp_manual));
  }
//TODO spot AE
  if(_config.backlight_compensation == 1)
  {
    viscaSerial.set_backlight_comp_power(&iface, &camera, CAM_BackLightModeInq_On);
  }
  else
  {
    viscaSerial.set_backlight_comp_power(&iface, &camera, CAM_BackLightModeInq_Off);
  }
  if(_config.spotAE == 1)
  {
    viscaSerial.set_spot_ae_power(&iface, &camera, CAM_SpotAE_On);
    viscaSerial.set_spot_ae_position(&iface, &camera,_config.spotAE_position_x,_config.spotAE_position_y);
  }
  else
  {
    viscaSerial.set_spot_ae_power(&iface, &camera, CAM_SpotAE_Off);
  }
  viscaSerial.set_ae_response_direct(&iface, &camera, _config.AE_response);
  if(_config.wide_dynamic_range == 1)
  {
    viscaSerial.set_wide_dynamic_power(&iface, &camera,CAM_WD_On);
  }
  else
  {
    viscaSerial.set_exp_comp_power(&iface, &camera,CAM_WD_Off);
  }
  switch (_config.defog)
  {
  case 0: viscaSerial.set_defog_off(&iface, &camera);
  break;
  case 1: viscaSerial.set_defog_on(&iface, &camera,CAM_Defog_Low);
  break;
  case 2: viscaSerial.set_defog_on(&iface, &camera,CAM_Defog_Mid);
  break;
  case 3: viscaSerial.set_defog_on(&iface, &camera,CAM_Defog_High);
  break;
  default: viscaSerial.set_defog_off(&iface, &camera);
  }
  viscaSerial.set_aperture_direct(&iface, &camera,_config.aperture_direct);
  viscaSerial.set_noise_reduction(&iface, &camera,_config.value3DNR,_config.value2DNR);
  switch (_config.gamma)
  {
  case 0: viscaSerial.set_gamma(&iface, &camera,CAM_Gamma_Standard);
  break;
  case 1: viscaSerial.set_gamma(&iface, &camera,CAM_Gamma_Streight);
  break;
  case 2: viscaSerial.set_gamma(&iface, &camera,CAM_Gamma_LowNoise);
  break;
  case 3: viscaSerial.set_gamma(&iface, &camera,CAM_Gamma_WDR);
  break;
  default: viscaSerial.set_gamma(&iface, &camera,CAM_Gamma_Standard);
  }
  if(_config.lr_reverse == 1)
  {
    viscaSerial.set_lr_reverse_power(&iface, &camera,CAM_LR_Reverse_On);
  }
  else
  {
    viscaSerial.set_lr_reverse_power(&iface, &camera,CAM_LR_Reverse_Off);
  }
  if(_config.freeze == 1)
  {
    viscaSerial.set_freeze_power(&iface, &camera,CAM_FREEZE_On);
  }
  else
  {
    viscaSerial.set_freeze_power(&iface, &camera,CAM_FREEZE_Off);
  }
  if(_config.picture_effect == 1)
  {
    viscaSerial.set_picture_effect(&iface, &camera,CAM_PictureEffect_BW);
  }
  else
  {
    viscaSerial.set_picture_effect(&iface, &camera,CAM_PictureEffect_Off);
  }
  if(_config.picture_flip == 1)
  {
    viscaSerial.set_pictureflip_power(&iface, &camera,CAM_PictureFlip_On);
  }
  else
  {
    viscaSerial.set_pictureflip_power(&iface, &camera,CAM_PictureFlip_Off);
  }
  switch (_config.chroma_suppression)
  {
  case 0: viscaSerial.set_chroma_suppression(&iface, &camera,CAM_ChromaSuppress_Low);
  break;
  case 1: viscaSerial.set_gamma(&iface, &camera,CAM_ChromaSuppress_Level1);
  break;
  case 2: viscaSerial.set_gamma(&iface, &camera,CAM_ChromaSuppress_Level2);
  break;
  case 3: viscaSerial.set_gamma(&iface, &camera,CAM_ChromaSuppress_Level3);
  break;
  default: viscaSerial.set_gamma(&iface, &camera,CAM_ChromaSuppress_Level2);
  }
  viscaSerial.set_color_gain_direct(&iface, &camera,_config.color_gain);
  viscaSerial.set_color_hue_direct(&iface, &camera,_config.color_hue);

  new_value = 0;
	  }
    std_msgs::UInt16 zoom_value;
    viscaSerial.get_zoom_value(&iface, &camera,&zoom_value.data);
    zoom_pub.publish(zoom_value);

 // ViscaSerial.VISCA_set_zoom_value()
 // f.

  //ROS_INFO("Spinning node");
  ros::spinOnce();
  rate.sleep();
  }
}

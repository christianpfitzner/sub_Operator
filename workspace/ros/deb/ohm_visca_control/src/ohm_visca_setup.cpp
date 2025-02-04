/*
 * ohm_visca_setup.cpp
 *
 *  Created on: 15.01.2019
 *      Author: volletjo
 */

#include <ros/ros.h>
#include "ViscaSerial.h"
#include <dynamic_reconfigure/server.h>
#include <ohm_visca_control/ohm_visca_setupConfig.h>

ohm_visca_control::ohm_visca_setupConfig _config;

VISCAInterface_t iface;
VISCACamera_t camera;
const char *device_name = "/dev/tamron-zoom-control";
bool apply_data_old = 0;
void callback(ohm_visca_control::ohm_visca_setupConfig &config, uint32_t level) {
  ROS_INFO("Reconfigure Request: %d,%d,%s,%i,%d,%d,%d,%d,%d,%d,%d,%d,%s",
      config.baudrate,
      config.d_zoom_max,
      config.zoom_tracking_AF?"True":"False",
      config.keystone_correction,
      config.monitoring_mode,
      config.LVDS_mode,
      config.lense_comp_upper,
      config.lense_comp_lower,
      config.maximum_f,
      config.minimum_f,
      config.minimum_shutter_speed,
      config.maximum_shutter_speed,
      config.apply_data?"True":"False");
  _config = config;
}

int main(int argc, char **argv)
{

 ros::init(argc, argv, "ohm_visca_setup");
 ros::NodeHandle n;
 dynamic_reconfigure::Server<ohm_visca_control::ohm_visca_setupConfig> server;
 dynamic_reconfigure::Server<ohm_visca_control::ohm_visca_setupConfig>::CallbackType f;

  f = boost::bind(&callback, _1, _2);
  server.setCallback(f);

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
      if ((apply_data_old == 0) && (_config.apply_data == 1))
      {
        uint8_t baudrate = CAM_RegisterValue_baud_9600;
        switch (_config.baudrate)
        {
        case 0: baudrate=CAM_RegisterValue_baud_9600;
        break;
        case 1: baudrate=CAM_RegisterValue_baud_19200;
        break;
        case 2: baudrate=CAM_RegisterValue_baud_38400;
        break;
        case 3: baudrate=CAM_RegisterValue_baud_115200;
        break;
        default: baudrate=CAM_RegisterValue_baud_9600;
        }
        viscaSerial.set_register(&iface, &camera, CAM_RegisterValue_baud, baudrate);

        uint8_t d_zoom_max = CAM_RegisterValue_DZoomMax_x12;
        switch (_config.d_zoom_max)
        {
        case 0: d_zoom_max=CAM_RegisterValue_DZoomMax_x1;
        break;
        case 1: d_zoom_max=CAM_RegisterValue_DZoomMax_x4;
        break;
        case 2: d_zoom_max=CAM_RegisterValue_DZoomMax_x12;
        break;
        case 3: d_zoom_max=CAM_RegisterValue_DZoomMax_x16;
        break;
        default: d_zoom_max=CAM_RegisterValue_DZoomMax_x12;
        }
        viscaSerial.set_register(&iface, &camera, CAM_RegisterValue_DZoomMax, d_zoom_max);

        if(_config.zoom_tracking_AF == 1)
        {
          viscaSerial.set_register(&iface, &camera, CAM_RegisterValue_ZoomTracking, CAM_RegisterValue_ZoomTracking_On);
        }
        else
        {
          viscaSerial.set_register(&iface, &camera, CAM_RegisterValue_ZoomTracking, CAM_RegisterValue_ZoomTracking_Off);
        }

        viscaSerial.set_register(&iface, &camera, CAM_RegisterValue_KeystoneCorrection, _config.keystone_correction);

        uint8_t monitoring_mode = CAM_RegisterValue_MonitorMode_1080_30p;
        switch (_config.monitoring_mode)
        {
        case 0: monitoring_mode=CAM_RegisterValue_MonitorMode_1080_30p;
        break;
        case 1: monitoring_mode=CAM_RegisterValue_MonitorMode_1080_25p;
        break;
        case 2: monitoring_mode=CAM_RegisterValue_MonitorMode_1080_60i;
        break;
        case 3: monitoring_mode=CAM_RegisterValue_MonitorMode_1080_50i;
        break;
        case 4: monitoring_mode=CAM_RegisterValue_MonitorMode_720_60p;
        break;
        case 5: monitoring_mode=CAM_RegisterValue_MonitorMode_720_50p;
        break;
        case 6: monitoring_mode=CAM_RegisterValue_MonitorMode_1080_60p;
        break;
        case 7: monitoring_mode=CAM_RegisterValue_MonitorMode_1080_50p;
        break;
        case 8: monitoring_mode=CAM_RegisterValue_MonitorMode_720_59_94p;
        break;
        case 9: monitoring_mode=CAM_RegisterValue_MonitorMode_NTSC;
        break;
        case 10: monitoring_mode=CAM_RegisterValue_MonitorMode_PAL;
        break;
        case 11: monitoring_mode=CAM_RegisterValue_MonitorMode_720_29_97p;
        break;
        case 12: monitoring_mode=CAM_RegisterValue_MonitorMode_720_30p;
        break;
        case 13: monitoring_mode=CAM_RegisterValue_MonitorMode_720_25p;
        break;
        case 14: monitoring_mode=CAM_RegisterValue_MonitorMode_1080_59_94p;
        break;
        case 15: monitoring_mode=CAM_RegisterValue_MonitorMode_1080_29_97p;
        break;
        case 16: monitoring_mode=CAM_RegisterValue_MonitorMode_1080_59_94i;
        break;
        default: monitoring_mode=CAM_RegisterValue_MonitorMode_1080_30p;
        }
        viscaSerial.set_register(&iface, &camera, CAM_RegisterValue_MonitorMode, monitoring_mode);

        uint8_t LVDS_mode = CAM_RegisterValue_LVDSmode_Single;
        switch (_config.LVDS_mode)
        {
        case 0: LVDS_mode=CAM_RegisterValue_LVDSmode_Single;
        break;
        case 1: LVDS_mode=CAM_RegisterValue_LVDSmode_Dual;
        break;
        default: LVDS_mode=CAM_RegisterValue_LVDSmode_Single;
        }
        viscaSerial.set_register(&iface, &camera, CAM_RegisterValue_LVDSmode, LVDS_mode);

        uint8_t lense_comp_upper = CAM_RegisterValue_LenseDistComp_Crop;
        switch (_config.lense_comp_upper)
        {
        case 0: lense_comp_upper=CAM_RegisterValue_LenseDistComp_Crop;
        break;
        case 1: lense_comp_upper=CAM_RegisterValue_LenseDistComp_All;
        break;
        default: lense_comp_upper=CAM_RegisterValue_LenseDistComp_Crop;
        }
        uint8_t lense_comp_lower = CAM_RegisterValue_LenseDistComp_Table1;
        switch (_config.lense_comp_lower)
        {
        case 0: lense_comp_upper=CAM_RegisterValue_LenseDistComp_Off;
        break;
        case 1: lense_comp_upper=CAM_RegisterValue_LenseDistComp_Table1;
        break;
        case 2: lense_comp_upper=CAM_RegisterValue_LenseDistComp_Table2;
        break;
        case 3: lense_comp_upper=CAM_RegisterValue_LenseDistComp_Table3;
        break;
        case 4: lense_comp_upper=CAM_RegisterValue_LenseDistComp_Table4;
        break;
        default: lense_comp_upper=CAM_RegisterValue_LenseDistComp_Table1;
        }
        viscaSerial.set_register(&iface, &camera, CAM_RegisterValue_LenseDistComp, ((lense_comp_upper << 4) | lense_comp_lower));

        uint8_t maximum_f = CAM_RegisterValue_MaxFnumber;
        switch (_config.maximum_f)
        {
        case 0: maximum_f=CAM_RegisterValue_MaxFnumber_F22_0;
        break;
        case 1: maximum_f=CAM_RegisterValue_MaxFnumber_F16_0;
        break;
        case 2: maximum_f=CAM_RegisterValue_MaxFNumber_F14_0;
        break;
        case 3: maximum_f=CAM_RegisterValue_MaxFnumber_F11_0;
        break;
        case 4: maximum_f=CAM_RegisterValue_MaxFnumber_F9_6;
        break;
        case 5: maximum_f=CAM_RegisterValue_MaxFnumber_F8_0;
        break;
        default: maximum_f=CAM_RegisterValue_MaxFnumber_F11_0;
        }
        viscaSerial.set_register(&iface, &camera, CAM_RegisterValue_MaxFnumber, maximum_f);

        uint8_t minimum_f = CAM_RegisterValue_FnumberSmallAperture_F4_0;
        switch (_config.minimum_f)
        {
        case 0: minimum_f=CAM_RegisterValue_FnumberSmallAperture_F6_8;
        break;
        case 1: minimum_f=CAM_RegisterValue_FnumberSmallAperture_F5_6;
        break;
        case 2: minimum_f=CAM_RegisterValue_FnumberSmallAperture_F4_8;
        break;
        case 3: minimum_f=CAM_RegisterValue_FnumberSmallAperture_F4_0;
        break;
        case 4: minimum_f=CAM_RegisterValue_FnumberSmallAperture_F3_4;
        break;
        case 5: minimum_f=CAM_RegisterValue_FnumberSmallAperture_F2_8;
        break;
        case 6: minimum_f=CAM_RegisterValue_FnumberSmallAperture_F2_4;
        break;
        case 7: minimum_f=CAM_RegisterValue_FnumberSmallAperture_F2_0;
        break;
        case 8: minimum_f=CAM_RegisterValue_FnumberSmallAperture_F1_8;
        break;
        default: minimum_f=CAM_RegisterValue_FnumberSmallAperture_F4_0;
        }
        viscaSerial.set_register(&iface, &camera, CAM_RegisterValue_FnumberSmallAperture, minimum_f);

        uint8_t minimum_shutter_speed = CAM_RegisterValue_MinimumShutterSpeed_1_1000;
        switch (_config.minimum_shutter_speed)
        {
        case 0: minimum_shutter_speed=CAM_RegisterValue_MinimumShutterSpeed_1_250;
        break;
        case 1: minimum_shutter_speed=CAM_RegisterValue_MinimumShutterSpeed_1_350;
        break;
        case 2: minimum_shutter_speed=CAM_RegisterValue_MinimumShutterSpeed_1_500;
        break;
        case 3: minimum_shutter_speed=CAM_RegisterValue_MinimumShutterSpeed_1_725;
        break;
        case 4: minimum_shutter_speed=CAM_RegisterValue_MinimumShutterSpeed_1_1000;
        break;
        case 5: minimum_shutter_speed=CAM_RegisterValue_MinimumShutterSpeed_1_1500;
        break;
        case 6: minimum_shutter_speed=CAM_RegisterValue_MinimumShutterSpeed_1_2000;
        break;
        case 7: minimum_shutter_speed=CAM_RegisterValue_MinimumShutterSpeed_1_3000;
        break;
        case 8: minimum_shutter_speed=CAM_RegisterValue_MinimumShutterSpeed_1_4000;
        break;
        case 9: minimum_shutter_speed=CAM_RegisterValue_MinimumShutterSpeed_1_6000;
        break;
        case 10: minimum_shutter_speed=CAM_RegisterValue_MinimumShutterSpeed_1_10000;
        break;
        default: minimum_shutter_speed=CAM_RegisterValue_MinimumShutterSpeed_1_1000;
        }
        viscaSerial.set_register(&iface, &camera, CAM_RegisterValue_MinimumShutterSpeed, minimum_shutter_speed);

        uint8_t maximum_shutter_speed = CAM_RegisterValue_MaximumShutterSpeed_1_4;
        switch (_config.maximum_shutter_speed)
        {
        case 0: maximum_shutter_speed=CAM_RegisterValue_MaximumShutterSpeed_1_1;
        break;
        case 1: maximum_shutter_speed=CAM_RegisterValue_MaximumShutterSpeed_1_2;
        break;
        case 2: maximum_shutter_speed=CAM_RegisterValue_MaximumShutterSpeed_1_3;
        break;
        case 3: maximum_shutter_speed=CAM_RegisterValue_MaximumShutterSpeed_1_4;
        break;
        case 4: maximum_shutter_speed=CAM_RegisterValue_MaximumShutterSpeed_1_6;
        break;
        case 5: maximum_shutter_speed=CAM_RegisterValue_MaximumShutterSpeed_1_8;
        break;
        case 6: maximum_shutter_speed=CAM_RegisterValue_MaximumShutterSpeed_1_15;
        break;
        default: maximum_shutter_speed=CAM_RegisterValue_MaximumShutterSpeed_1_4;
        }
        viscaSerial.set_register(&iface, &camera, CAM_RegisterValue_MaximumShutterSpeed, maximum_shutter_speed);
        std::cout<<"data_send please restart device" << std::endl;
      }
      apply_data_old = _config.apply_data;
      ros::spinOnce();
      rate.sleep();
    }
    return 0;
}

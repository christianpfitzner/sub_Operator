/*
 * ViscaSerial.h
 *
 *  Created on: 06.12.2018
 *      Author: volletjo
 */

#ifndef VISCASERIAL_H_
#define VISCASERIAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <chrono>
#include <thread>

/*Visca Protocol essentials*/
#define CAM_Command 							                      0x01
#define CAM_Inquiry								                      0x09
#define CAM_Category_Interface							            0x00
#define CAM_Category_Camera1								            0x04
#define CAM_Category_Pan_Tilter							            0x06
#define CAM_Category_Camera2								            0x07
#define CAM_Terminator							                    0xFF

#define CAM_Camera_Address						                  0x81
#define CAM_Camera_Broadcast					                  0x88

#define CAM_Camera_ACK_Address					                0x90

#define CAM_RESPONSE_CLEAR                              0x40
#define CAM_RESPONSE_ADDRESS                            0x30
#define CAM_RESPONSE_ACK                                0x40
#define CAM_RESPONSE_COMPLETED                          0x50
#define CAM_RESPONSE_ERROR                              0x60

#define IF_Clear								                        0x01

#define CAM_Error_Syntax						                    0x02
#define CAM_Error_BufferFull					                  0x03
#define CAM_Error_CommandCanceled				                0x04
#define CAM_Error_NoSocket						                  0x05
#define CAM_Error_CommandNotExecutable			            0x41

/*commands (order follows the Tamron MP1110M-VC001E-1 Documentation)*/
#define CAM_Power 				                              0x00
#define CAM_Power_Off                                   0x03

/*Analog Zoom*/
#define CAM_Zoom						                            0x07
#define CAM_Zoom_Stop					                          0x00
#define CAM_Zoom_Tele					                          0x02
#define CAM_Zoom_Wide					                          0x03
#define	CAM_Zoom_Tele_VarSpeed			                    0x20
#define CAM_Zoom_Wide_VarSpeed 			                    0x30
#define CAM_Zoom_Direct					                        0x47

/*Digital Zoom*/
#define CAM_Dzoom						                            0x06
#define CAM_Dzoom_On					                          0x02
#define CAM_Dzoom_Off					                          0x03
#define CAM_Dzoom_x1					                          0x4000
#define CAM_Dzoom_x2					                          0x6000
#define CAM_Dzoom_x4					                          0x7000
#define CAM_Dzoom_x8					                          0x7800
#define CAM_Dzoom_x12					                          0x7AC0
#define CAM_Dzoom_x16					                          0x7C00

/*Focus*/
#define CAM_Focus						                            0x08
#define CAM_Focus_Stop					                        0x00
#define CAM_Focus_Far					                          0x02
#define CAM_Focus_Near					                        0x03
#define CAM_Focus_Far_VarSpeed			                    0x20
#define CAM_Focus_Near_VarSpeed			                    0x30
#define CAM_Focus_Direct				                        0x48
#define CAM_Focus_Auto					                        0x38
#define CAM_Focus_Auto_On				                        0x02
#define CAM_Focus_Auto_Off				                      0x03
#define CAM_Focus_Auto_Toggle			                      0x10
#define CAM_Focus_One_Push				                      0x18
#define CAM_Focus_One_Push_Trigger		                  0x01
#define CAM_Focus_Near_Limit			                      0x28

/*AF Sensitivity*/
#define CAM_AF_Sensitivity				                      0x58
#define CAM_AF_Sensitivity_Normal		                    0x02
#define CAM_AF_Sensitivity_Low			                    0x03

/*AF Mode*/
#define CAM_AFMode						                          0x57
#define CAM_AFMode_Normal				                        0x00
#define CAM_AFMode_Interval				                      0x01
#define CAM_AFMode_Zoom_Trigger			                    0x02
#define CAM_AFMode_Time					                        0x27

/*Direct Zoom and Focus*/
#define CAM_ZoomFocus_Direct			                      0x47

/*Vibration Compensation*/
#define CAM_VibrationCompensation		                    0x34
#define CAM_VibrationCompensation_On	                  0x02
#define CAM_VibrationCompensation_Off	                  0x03

/*White Balance*/
#define CAM_WB							                            0x35
#define CAM_WB_ATW_Narrow				                        0x00
#define CAM_WB_Indoor					                          0x01
#define CAM_WB_Outdoor					                        0x02
#define CAM_WB_One_Push_WB				                      0x03
#define CAM_WB_ATW_Wide					                        0x04
#define CAM_WB_Manual 					                        0x05
#define CAM_WB_One_Push					                        0x10
#define CAM_WB_One_Push_Trigger			                    0x05

/*Red Gain*/
#define CAM_RGain						                            0x03
#define CAM_RGain_Reset					                        0x00
#define CAM_RGain_Up					                          0x02
#define CAM_RGain_Down					                        0x03
#define CAM_RGain_Direct				                        0x43

/*Blue Gain*/
#define CAM_BGain						                            0x04
#define CAM_BGain_Reset					                        0x00
#define CAM_BGain_Up					                          0x02
#define CAM_BGain_Down					                        0x03
#define CAM_BGain_Direct				                        0x44

/*Automatic Exposure*/
#define CAM_AE							                            0x39
#define CAM_AE_Full_Auto				                        0x00
#define CAM_AE_Manual					                          0x03
#define CAM_AE_Shutter_Priority			                    0x0A
#define CAM_AE_Iris_Priority			                      0x0B

/*Slow Shutter*/
#define CAM_SlowShutter					                        0x5A
#define CAM_SlowShutter_Auto			                      0x02
#define CAM_SlowShutter_Manual			                    0x03

/*Shutter*/
#define CAM_Shutter						                          0x0A
#define CAM_Shutter_Reset				                        0x00
#define CAM_Shutter_Up					                        0x02
#define CAM_Shutter_Down				                        0x03
#define CAM_Shutter_Direct				                      0x4A
#define CAM_Shutter_Direct_1_1			                    0x00
#define CAM_Shutter_Direct_1_2			                    0x01
#define CAM_Shutter_Direct_1_4			                    0x02
#define CAM_Shutter_Direct_1_8			                    0x03
#define CAM_Shutter_Direct_1_15			                    0x04
#define CAM_Shutter_Direct_1_30			                    0x05
#define CAM_Shutter_Direct_1_60			                    0x06
#define CAM_Shutter_Direct_1_90			                    0x07
#define CAM_Shutter_Direct_1_100		                    0x08
#define CAM_Shutter_Direct_1_120		                    0x09
#define CAM_Shutter_Direct_1_180		                    0x0A
#define CAM_Shutter_Direct_1_250		                    0x0B
#define CAM_Shutter_Direct_1_350		                    0x0C
#define CAM_Shutter_Direct_1_500		                    0x0D
#define CAM_Shutter_Direct_1_725		                    0x0E
#define CAM_Shutter_Direct_1_1000		                    0x0F
#define CAM_Shutter_Direct_1_1500		                    0x10
#define CAM_Shutter_Direct_1_2000		                    0x11
#define CAM_Shutter_Direct_1_3000		                    0x12
#define CAM_Shutter_Direct_1_4000		                    0x13
#define CAM_Shutter_Direct_1_6000		                    0x14
#define CAM_Shutter_Direct_1_10000	                    0x15

/*Iris*/
#define CAM_Iris						                            0x0B
#define CAM_Iris_Reset					                        0x00
#define CAM_Iris_Up						                          0x02
#define CAM_Iris_Down					                          0x03
#define CAM_Iris_Direct					                        0x4B
#define CAM_Iris_Direct_Close			                      0x00
#define CAM_Iris_Direct_F22_0			                      0x03
#define CAM_Iris_Direct_F16_0			                      0x04
#define CAM_Iris_Direct_F14_0			                      0x05
#define CAM_Iris_Direct_F11_0			                      0x06
#define CAM_Iris_Direct_F9_6			                      0x07
#define CAM_Iris_Direct_F8_0			                      0x08
#define CAM_Iris_Direct_F6_8			                      0x09
#define CAM_Iris_Direct_F5_6			                      0x0A
#define CAM_Iris_Direct_F4_8			                      0x0B
#define CAM_Iris_Direct_F4_0			                      0x0C
#define CAM_Iris_Direct_F3_4			                      0x0D
#define CAM_Iris_Direct_F2_8			                      0x0E
#define CAM_Iris_Direct_F2_4			                      0x0F
#define CAM_Iris_Direct_F2_0			                      0x10
#define CAM_Iris_Direct_F1_8			                      0x11

/*Gain*/
#define CAM_Gain						                            0x0C
#define CAM_Gain_Reset				   	                      0x00
#define CAM_Gain_Up						                          0x02
#define CAM_Gain_Down					                          0x03
#define CAM_Gain_Direct					                        0x4C
#define CAM_Gain_Direct_0dB			 	                      0x00
#define CAM_Gain_Direct_p0dB		 	                      0x01
#define CAM_Gain_Direct_p2dB		 	                      0x02
#define CAM_Gain_Direct_p4dB		 	                      0x03
#define CAM_Gain_Direct_p6dB		 	                      0x04
#define CAM_Gain_Direct_p8dB		 	                      0x05
#define CAM_Gain_Direct_p10dB		 	                      0x06
#define CAM_Gain_Direct_p12dB		 	                      0x07
#define CAM_Gain_Direct_p14dB		 	                      0x08
#define CAM_Gain_Direct_p16dB		 	                      0x09
#define CAM_Gain_Direct_p18dB		 	                      0x0A
#define CAM_Gain_Direct_p20dB		 	                      0x0B
#define CAM_Gain_Direct_p22dB		 	                      0x0C
#define CAM_Gain_Direct_p24dB		 	                      0x0D
#define CAM_Gain_Direct_p26dB		 	                      0x0E
#define CAM_Gain_Direct_p28dB		 	                      0x0F
#define CAM_Gain_Direct_p30dB		 	                      0x10
#define CAM_Gain_Direct_p32dB		 	                      0x11
#define CAM_Gain_Direct_p34dB		 	                      0x12
#define CAM_Gain_Direct_p36dB		 	                      0x13
#define CAM_Gain_Direct_p38dB		 	                      0x14
#define CAM_Gain_Direct_p40dB		 	                      0x15
#define CAM_Gain_Direct_p42dB		 	                      0x16
#define CAM_Gain_Direct_p44dB		 	                      0x17
#define CAM_Gain_Direct_p46dB		 	                      0x18
#define CAM_Gain_Limit					                        0x2C

/*Exposure Compensation*/
#define CAM_ExpComp_Enable				                      0x3E
#define CAM_ExpComp_Enable_On			                      0x02
#define CAM_ExpComp_Enable_Off			                    0x03
#define CAM_ExpComp						                          0x0E
#define CAM_ExpComp_Reset				                        0x00
#define CAM_ExpComp_Up					                        0x02
#define CAM_ExpComp_Down				                        0x03
#define CAM_ExpComp_Direct				                      0x4E
#define CAM_ExpComp_Direct_m12dB	                    	0x01
#define CAM_ExpComp_Direct_m10dB	                    	0x02
#define CAM_ExpComp_Direct_m8dB		                    	0x03
#define CAM_ExpComp_Direct_m6dB		                    	0x04
#define CAM_ExpComp_Direct_m4dB		                    	0x05
#define CAM_ExpComp_Direct_m2dB		                    	0x06
#define CAM_ExpComp_Direct_0dB		                    	0x07
#define CAM_ExpComp_Direct_p2dB		                    	0x08
#define CAM_ExpComp_Direct_p4dB		                    	0x09
#define CAM_ExpComp_Direct_p6dB		                    	0x0A
#define CAM_ExpComp_Direct_p8dB		                    	0x0B
#define CAM_ExpComp_Direct_p10dB	                    	0x0C
#define CAM_ExpComp_Direct_p12dB	                    	0x0D

/*Back Light Compensation*/
#define CAM_BackLightModeInq				            				0x33
#define CAM_BackLightModeInq_On			            			  0x02
#define CAM_BackLightModeInq_Off		            			  0x03

/*Spot Automatic Exposure*/
#define CAM_SpotAE									            		    0x59
#define CAM_SpotAE_On								            		    0x02
#define CAM_SpotAE_Off							            			  0x03
#define CAM_SpotAE_Position					            				0x29

/*Automatic Exposure Response*/
#define CAM_AE_Response_Direct			            				0x5D

/*Wide Dynamic Range*/
#define CAM_WD											            	      0x3D
#define CAM_WD_On										            	      0x02
#define CAM_WD_Off									            		    0x03

/*Defog*/
#define CAM_Defog										            	      0x37
#define CAM_Defog_On										                0x02
#define CAM_Defog_Off										                0x03
#define CAM_Defog_Low                                   0x01
#define CAM_Defog_Mid                                   0x02
#define CAM_Defog_High                                  0x03

/*Aperture/Sharpness*/
#define CAM_Aperture								            		    0x02
#define CAM_Aperture_Reset					            				0x00
#define CAM_Aperture_Up							            			  0x02
#define CAM_Aperture_Down						            			  0x03
#define CAM_Aperture_Direct					            				0x42

/*Noise Reduction*/
#define CAM_NR											                  	0x53

/*Gamma*/
#define CAM_Gamma										                  	0x5B
#define CAM_Gamma_Standard					            				0x00
#define CAM_Gamma_Streight					            				0x01
#define CAM_Gamma_LowNoise					            				0x02
#define CAM_Gamma_WDR								            		    0x03

/*Mirror*/
#define CAM_LR_Reverse							            			  0x61
#define CAM_LR_Reverse_On						            			  0x02
#define CAM_LR_Reverse_Off					            				0x03

/*Video Freeze*/
#define CAM_FREEZE									            		    0x62
#define CAM_FREEZE_On								            		    0x02
#define CAM_FREEZE_Off							            			  0x03

/*Picture Effect*/
#define CAM_PictureEffect						            			  0x63
#define CAM_PictureEffect_Off				            				0x00
#define CAM_PictureEffect_BW				            				0x04

/*Picture Flip*/
#define CAM_PictureFlip							            			  0x66
#define CAM_PictureFlip_On					            				0x02
#define CAM_PictureFlip_Off									            0x03

/*Memory*/
#define CAM_Memory											                0x3F
#define CAM_Memory_Reset									              0x00
#define CAM_Memory_Set										              0x01
#define CAM_Memory_Recall									              0x02
#define CAM_Memory_Memory0									            0x00
#define CAM_Memory_Memory1									            0x01
#define CAM_Memory_Memory2									            0x02
#define CAM_Memory_Memory3									            0x03

/*Custom Mode at Startup*/
#define CAM_CUSTOM											                0x3F
#define CAM_CUSTOM_Reset									              0x00
#define CAM_Custom_Set										              0x01
#define CAM_Custom_Recall									              0x02

/*Mute*/
#define CAM_Mute											                  0x75
#define CAM_Mute_On											                0x02
#define CAM_Mute_Off										                0x03

/*ZoomPosition Continuous Output*/
#define CAM_ContinuousZoomPosReply							        0x69
#define CAM_ContinuousZoomPosReply_On						        0x02
#define CAM_ContinuousZoomPosReply_Off						      0x03

/*Setting the Reply Interval*/
#define CAM_ReplyIntervalTimeSet							          0x6A

/* Configuration of the Camera*/
#define CAM_RegisterValue									              0x24
#define CAM_RegisterValue_baud								          0x00
#define CAM_RegisterValue_baud_9600							        0x00
#define CAM_RegisterValue_baud_19200						        0x01
#define CAM_RegisterValue_baud_38400						        0x02
#define CAM_RegisterValue_baud_115200						        0x03
#define CAM_RegisterValue_DZoomMax							        0x52
#define CAM_RegisterValue_DZoomMax_x1						        0x00
#define CAM_RegisterValue_DZoomMax_x4						        0xC0
#define CAM_RegisterValue_DZoomMax_x12						      0xEB
#define CAM_RegisterValue_DZoomMax_x16						      0xF0
#define CAM_RegisterValue_ZoomTracking						      0x54
#define CAM_RegisterValue_ZoomTracking_Off					    0x00
#define CAM_RegisterValue_ZoomTracking_On					      0x01
#define CAM_RegisterValue_KeystoneCorrection				    0x60
#define CAM_RegisterValue_KeystoneCorrection_Off			  0x00
#define CAM_RegisterValue_KeystoneCorrection_Low			  0x01
#define CAM_RegisterValue_KeystoneCorrection_High			  0x12
#define CAM_RegisterValue_MonitorMode						        0x72
#define CAM_RegisterValue_MonitorMode_1080_30p				  0x01
#define CAM_RegisterValue_MonitorMode_1080_25p				  0x02
#define CAM_RegisterValue_MonitorMode_1080_60i				  0x03
#define CAM_RegisterValue_MonitorMode_1080_50i				  0x04
#define CAM_RegisterValue_MonitorMode_720_60p			  	  0x05
#define CAM_RegisterValue_MonitorMode_720_50p				    0x06
#define CAM_RegisterValue_MonitorMode_1080_60p				  0x07
#define CAM_RegisterValue_MonitorMode_1080_50p				  0x08
#define CAM_RegisterValue_MonitorMode_720_59_94p			  0x09
#define CAM_RegisterValue_MonitorMode_NTSC				  	  0x0B
#define CAM_RegisterValue_MonitorMode_PAL					      0x0D
#define CAM_RegisterValue_MonitorMode_720_29_97p			  0x0E
#define CAM_RegisterValue_MonitorMode_720_30p				    0x0F
#define CAM_RegisterValue_MonitorMode_720_25p				    0x11
#define CAM_RegisterValue_MonitorMode_1080_59_94p			  0x13
#define CAM_RegisterValue_MonitorMode_1080_29_97p			  0x14
#define CAM_RegisterValue_MonitorMode_1080_59_94i			  0x15
#define CAM_RegisterValue_LVDSmode							        0x74
#define CAM_RegisterValue_LVDSmode_Single					      0x00
#define CAM_RegisterValue_LVDSmode_Dual						      0x01
#define CAM_RegisterValue_LenseDistComp						      0x78
#define CAM_RegisterValue_LenseDistComp_Crop				    0x00
#define CAM_RegisterValue_LenseDistComp_All					    0x01
#define CAM_RegisterValue_LenseDistComp_Off					    0x00
#define CAM_RegisterValue_LenseDistComp_Table1				  0x01
#define CAM_RegisterValue_LenseDistComp_Table2				  0x02
#define CAM_RegisterValue_LenseDistComp_Table3				  0x03
#define CAM_RegisterValue_LenseDistComp_Table4				  0x04
#define CAM_RegisterValue_MaxFnumber						        0x79
#define CAM_RegisterValue_MaxFnumber_F22_0					    0x03
#define CAM_RegisterValue_MaxFnumber_F16_0					    0x04
#define CAM_RegisterValue_MaxFNumber_F14_0					    0x05
#define CAM_RegisterValue_MaxFnumber_F11_0					    0x06
#define CAM_RegisterValue_MaxFnumber_F9_6					      0x07
#define CAM_RegisterValue_MaxFnumber_F8_0					      0x08
#define CAM_RegisterValue_FnumberSmallAperture				  0x7A
#define CAM_RegisterValue_FnumberSmallAperture_F6_8		  0x09
#define CAM_RegisterValue_FnumberSmallAperture_F5_6		  0x0A
#define CAM_RegisterValue_FnumberSmallAperture_F4_8		  0x0B
#define CAM_RegisterValue_FnumberSmallAperture_F4_0		  0x0C
#define CAM_RegisterValue_FnumberSmallAperture_F3_4		  0x0D
#define CAM_RegisterValue_FnumberSmallAperture_F2_8		  0x0E
#define CAM_RegisterValue_FnumberSmallAperture_F2_4		  0x0F
#define CAM_RegisterValue_FnumberSmallAperture_F2_0		  0x10
#define CAM_RegisterValue_FnumberSmallAperture_F1_8		  0x11
#define CAM_RegisterValue_MinimumShutterSpeed				    0x7B
#define CAM_RegisterValue_MinimumShutterSpeed_1_250		  0x0B
#define CAM_RegisterValue_MinimumShutterSpeed_1_350		  0x0C
#define CAM_RegisterValue_MinimumShutterSpeed_1_500		  0x0D
#define CAM_RegisterValue_MinimumShutterSpeed_1_725		  0x0E
#define CAM_RegisterValue_MinimumShutterSpeed_1_1000	  0x0F
#define CAM_RegisterValue_MinimumShutterSpeed_1_1500	  0x10
#define CAM_RegisterValue_MinimumShutterSpeed_1_2000	  0x11
#define CAM_RegisterValue_MinimumShutterSpeed_1_3000	  0x12
#define CAM_RegisterValue_MinimumShutterSpeed_1_4000	  0x13
#define CAM_RegisterValue_MinimumShutterSpeed_1_6000	  0x14
#define CAM_RegisterValue_MinimumShutterSpeed_1_10000	  0x15
#define CAM_RegisterValue_MaximumShutterSpeed				    0x7C
//#define CAM_RegisterValue_MaximumShutterSpeed_1_4		  	0x00
#define CAM_RegisterValue_MaximumShutterSpeed_1_1			  0x01
#define CAM_RegisterValue_MaximumShutterSpeed_1_2			  0x02
#define CAM_RegisterValue_MaximumShutterSpeed_1_3			  0x03
#define CAM_RegisterValue_MaximumShutterSpeed_1_4			  0x04
#define CAM_RegisterValue_MaximumShutterSpeed_1_6			  0x05
#define CAM_RegisterValue_MaximumShutterSpeed_1_8			  0x06
#define CAM_RegisterValue_MaximumShutterSpeed_1_15		  0x07
#define CAM_RegisterValue_SetupControl						      0x7D
#define CAM_RegisterValue_SetupControl_ResetParameters  0x0D
#define CAM_RegisterValue_SetupControl_UpdateFirmware	  0x3A

/*Chroma Suppression*/
#define CAM_ChromaSuppress									            0x5F
#define CAM_ChromaSuppress_Low								          0x00
#define CAM_ChromaSuppress_Level1							          0x01
#define CAM_ChromaSuppress_Level2							          0x02
#define CAM_ChromaSuppress_Level3							          0x03

/*Color Gain / Saturation*/
#define CAM_ColorGain_Direct								            0x49

/*Color Hue*/
#define CAM_ColorHue_Direct									            0x4F

/*Lens temperature inquiry*/

#define CAM_Lens_Temp                                   0x68

/*Useful Stuff*/
#define	VISCA_SUCCESS										                0x00
#define VISCA_FAILURE									                  0xFF



#include <termios.h>
#include <stdint.h>

/* timeout in us */
#define VISCA_SERIAL_WAIT              100000

/* size of the local packet buffer */
#define VISCA_INPUT_BUFFER_SIZE          1024

/* This is the interface for the POSIX platform.
 */
typedef struct _VISCA_interface
{
  // RS232 data:
  int port_fd;
  struct termios options;
  uint32_t baud;

  // VISCA data:
  uint32_t address;
  uint32_t broadcast;

  // RS232 input buffer
  unsigned char ibuf[VISCA_INPUT_BUFFER_SIZE];
  uint32_t bytes;
  uint32_t type;

} VISCAInterface_t;

typedef struct _VISCA_interface *pVISCAInterface_t;


/* CAMERA STRUCTURE */
typedef struct _VISCACamera
{
  // CAM data:
  int address;

  // camera info:
  uint32_t vendor;
  uint32_t model;
  uint32_t rom_version;
  uint32_t socket_num;

} VISCACamera_t;


/* TITLE STRUCTURE */
typedef struct _VISCAtitle
{
  uint32_t vposition;
  uint32_t hposition;
  uint32_t color;
  uint32_t blink;
  unsigned char title[20];

} VISCATitleData_t;

typedef struct _VISCA_packet
{
  unsigned char bytes[32];
  uint32_t length;
} VISCAPacket_t;

class ViscaClass {
private:
  void append_byte(VISCAPacket_t *packet, unsigned char byte);
  void init_packet(VISCAPacket_t *packet);
  uint32_t write_packet_data(VISCAInterface_t *iface, VISCACamera_t *camera, VISCAPacket_t *packet);
  uint32_t get_reply(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t send_packet(VISCAInterface_t *iface, VISCACamera_t *camera, VISCAPacket_t *packet);
  uint32_t send_packet_with_reply(VISCAInterface_t *iface, VISCACamera_t *camera, VISCAPacket_t *packet);
  uint32_t get_packet(VISCAInterface_t *iface);
public:
  /* GENERAL FUNCTIONS */
  uint32_t open_serial(VISCAInterface_t *iface, const char *device_name);
  uint32_t close_serial(VISCAInterface_t *iface);
  uint32_t unread_bytes(VISCAInterface_t *iface, unsigned char *buffer, uint32_t *buffer_size);
  uint32_t usleep(uint32_t useconds);
  uint32_t set_address(VISCAInterface_t *iface, int *camera_num);
  uint32_t clear(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t get_version(VISCAInterface_t *iface, VISCACamera_t *camera);
  /* COMMAND FUNKTIONS */
  uint32_t set_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power);
  uint32_t set_zoom_tele(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_zoom_wide(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_zoom_stop(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_zoom_tele_speed(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t speed);
  uint32_t set_zoom_wide_speed(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t speed);
  uint32_t set_zoom_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t zoom);
  uint32_t set_digital_zoom_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power);
  uint32_t set_focus_stop(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_focus_far(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_focus_near(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_focus_far_speed(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t speed);
  uint32_t set_focus_near_speed(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t speed);
  uint32_t set_focus_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t focus);
  uint32_t set_focus_auto(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_focus_manual(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_focus_toggle(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_focus_one_push(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_focus_near_limit(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t limit);
  uint32_t set_focus_autosense_normal(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_focus_autosense_low(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_afmode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t mode);
  uint32_t set_afmode_time(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t movement_time, uint8_t interval_time);
  uint32_t set_zoom_and_focus_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t zoom, uint16_t focus);
  uint32_t set_vibration_compensation_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power);
  uint32_t set_whitebalance_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t mode);
  uint32_t set_whitebalance_one_push(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_rgain_reset(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_rgain_up(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_rgain_down(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_rgain_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value);
  uint32_t set_bgain_reset(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_bgain_up(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_bgain_down(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_bgain_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value);
  uint32_t set_auto_exp_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t mode);
  uint32_t set_slow_shutter_auto(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power);
  uint32_t set_shutter_reset(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_shutter_up(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_shutter_down(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_shutter_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value);
  uint32_t set_iris_reset(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_iris_up(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_iris_down(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_iris_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value);
  uint32_t set_gain_reset(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_gain_up(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_gain_down(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_gain_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value);
  uint32_t set_gain_limit(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value);
  uint32_t set_exp_comp_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power);
  uint32_t set_exp_comp_reset(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_exp_comp_up(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_exp_comp_down(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_exp_comp_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value);
  uint32_t set_backlight_comp_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power);
  uint32_t set_spot_ae_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power);
  uint32_t set_spot_ae_position(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t x_position, uint8_t y_position);
  uint32_t set_ae_response_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value);
  uint32_t set_wide_dynamic_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power);
  uint32_t set_defog_on(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value);
  uint32_t set_defog_off(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_aperture_reset(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_aperture_up(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_aperture_down(VISCAInterface_t *iface, VISCACamera_t *camera);
  uint32_t set_aperture_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value);
  uint32_t set_noise_reduction(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value3DNR, uint8_t value2DNR);
  uint32_t set_gamma(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value);
  uint32_t set_lr_reverse_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power);
  uint32_t set_freeze_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power);
  uint32_t set_picture_effect(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power);
  uint32_t set_pictureflip_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power);
  uint32_t memory_reset(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t channel);
  uint32_t memory_set(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t channel);
  uint32_t memory_recall(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t channel);
  uint32_t set_mute_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power);
  uint32_t set_continuous_zoom_pos_reply_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power);
  uint32_t set_reply_interval_time_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value);
  uint32_t set_register(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t reg_num, uint8_t reg_val);
  uint32_t set_chroma_suppression(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value);
  uint32_t set_color_gain_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value);
  uint32_t set_color_hue_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value);
  /* INQUIRY FUNKTIONS */
  uint32_t get_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power);
  uint32_t get_dzoom(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power);
  uint32_t get_zoom_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value);
  uint32_t get_focus_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value);
  uint32_t get_focus_auto(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power);
  uint32_t get_focus_near_limit(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value);
  uint32_t get_focus_auto_sensitivity(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode);
  uint32_t get_afmode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode);
  uint32_t get_afmode_time(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *movement_time, uint8_t *interval_time);
  uint32_t get_whitebalance_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode);
  uint32_t get_rgain_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value);
  uint32_t get_bgain_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value);
  uint32_t get_auto_exp_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode);
  uint32_t get_slow_shutter_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode);
  uint32_t get_shutter_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value);
  uint32_t get_iris_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value);
  uint32_t get_gain_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value);
  uint32_t get_gain_limit(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value);
  uint32_t get_exp_comp_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power);
  uint32_t get_exp_comp_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value);
  uint32_t get_backlight_comp_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power);
  uint32_t get_spot_ae_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power);
  uint32_t get_spot_ae_position(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *x_position, uint8_t *y_position);
  uint32_t get_ae_response_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value);
  uint32_t get_wide_dynamic_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power);
  uint32_t get_defog_on(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power);
  uint32_t get_aperture_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value);
  uint32_t get_noise_reduction(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value3DNR, uint8_t *value2DNR);
  uint32_t get_gamma(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value);
  uint32_t get_lr_reverse_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power);
  uint32_t get_FREEZE_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power);
  uint32_t get_picture_effect(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode);
  uint32_t get_pictureflip_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power);
  uint32_t get_memory(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *channel);
  uint32_t get_mute_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power);
  uint32_t get_continuous_zoom_pos_reply_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power);
  uint32_t get_reply_interval_time_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value);
  uint32_t get_register(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t reg_num, uint8_t *reg_val);
  uint32_t get_chroma_suppression(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value);
  uint32_t get_color_gain_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value);
  uint32_t get_color_hue_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value);
  uint32_t get_temperature(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value);
  uint32_t get_vibration_compensation_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power);
};

extern ViscaClass viscaSerial;
#endif /* SRC_VISCASERIAL_H_ */

/*
 * definitions.h
 *
 *  Created on: 05.03.2015
 *      Author: feesma44884
 */

#ifndef OHM_INSPECTOR_FIRMWARE_SRC_DEFINITIONS_H_
#define OHM_INSPECTOR_FIRMWARE_SRC_DEFINITIONS_H_

#include "core/inc/wirish.h"

/**
 * @brief: This file includes some major definitions or macros
 */

/**
 * @brief: Definitions for Hardware Timers
 */
#define HW_TIMER_DYNAMIXEL  1

/**
 * @brief: Definitions for Dynamixels
 */
#define DYNAMIXEL_MAX_RPM                   80.0f
#define DYNAMIXEL_MAX_ERROR_CNT             5
#define DYNAMIXEL_UPDATE_TIME_US            25000

/**
 * @brief: Definitions for Inspector
 */
#define INSPECTOR_L_0_MM   60.0f
#define INSPECTOR_L_1_MM   260.0f
#define INSPECTOR_L_2_MM   35.0f
#define INSPECTOR_L_3_MM   410.0f
#define INSPECTOR_L_4_MM   70.0f
#define INSPECTOR_L_5_MM   65.0f

#define INSPECTOR_MAX_ORIENT_SPEED          50.0f
#define INSPECTOR_MAX_SPEED                 300.0f
#define INSPECTOR_CONTROL_CALC_TIME_STEP    0.047f
#define INSPECTOR_CONTROL_MOVE_TIME_STEP    0.080f
#define INSPECTOR_ORIENT_UPDATE_TIME        0.0365f

#define INSPECTOR_COLLISION_DISTANCE_X      350.0f
#define INSPECTOR_COLLISION_DISTANCE_Y      50.0f

#define INSPECTOR_HOME_POSITION             {-1.57f, -0.8f, 1.57f, 0.0f}
//#define INSPECTOR_HOME_POSITION             {-1.57f, -0.8f, -1.55f, 0.0f}
#define INSPECTOR_PARK_POSITION             {-1.57f, -1.57f, -1.55f, 1.7f}

/**
 * @brief: Memory management makros
 */
#define SAFE_DELETE(x)  {if(x != NULL) {free(x);x = NULL;}}

/**
 * @brief: Convert makros
 */
union float_pack {
    float   f;
    uint8   bit[4];
};


#endif /* OHM_INSPECTOR_FIRMWARE_SRC_DEFINITIONS_H_ */

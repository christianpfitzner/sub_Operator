/*
 * definitions.h
 *
 *  Created on: 04.03.2015
 *      Author: feesma44884
 */

#ifndef OHM_INSPECTOR_SRC_DEFINITIONS_H_
#define OHM_INSPECTOR_SRC_DEFINITIONS_H_

#include <ros/ros.h>

/**
 * Define the lengths of the inspector
 */
#define INSPECTOR_L_0_M   0.060f
#define INSPECTOR_L_1_M   0.260f
#define INSPECTOR_L_2_M   0.035f
#define INSPECTOR_L_3_M   0.410f
#define INSPECTOR_L_4_M   0.070f
#define INSPECTOR_L_5_M   0.065f

/**
 * Macro to safe delete allocated memory
 */
#define SAFE_DELETE(x)  {if(x != NULL) {delete(x); x = NULL;}}

/**
 * Union to convert float variables to 4 x 8 bit vars
 */
union float_pack {
    float     f;        /**< Float value */
    uint8_t   bit[4];   /**< Float value saved in 4 x 8 bit vars */
};

#endif /* OHM_INSPECTOR_SRC_DEFINITIONS_H_ */

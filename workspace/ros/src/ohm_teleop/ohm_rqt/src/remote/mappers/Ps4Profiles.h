/*
 * Ps4Profiles.h
 *
 *  Created on: Apr 10, 2018
 *      Author: phil
 */

#ifndef OHM_SCHR_REMOTE_SRC_MAPPERS_PS4PROFILES_H_
#define OHM_SCHR_REMOTE_SRC_MAPPERS_PS4PROFILES_H_

/**
 * @brief AxisPs4
 * 
 */
enum class AxisPs4
 {
   A1_X = 0,         //!< x axis of left analog stick
   A1_Y,             //!< y axis of left analog stick
   L2,               //!< L2
   A2_X,             //!< x axis of right analog stick
   A2_Y,             //!< y axis of right analog stick
   R2,               //!< R2
   CROSS_X,         //!< command cross up
   CROSS_Y          //!< command cross right
 };

 /**
  * @brief ButtonsPs4
  * 
  */
 enum class ButtonsPs4
 {
   B_X = 0,              //!< cross button
   B_C,              //!< circle button
   B_T,              //!< triangle button
   B_S,              //!< square button
   B_L1,             //!< L1
   B_R1,             //!< R1
   B_L2,             //!< L2
   B_R2,             //!< R2
   B_SHARE,     //!< SHARE
   B_OPTIONS,     //!< OPTIONS
   B_PS,              //!< PS button
   B_A1,             //!< A1 left analog stick
   B_A2             //!< A2 right analog stick
 };

#endif /* OHM_SCHR_REMOTE_SRC_MAPPERS_PS3PROFILES_H_ */

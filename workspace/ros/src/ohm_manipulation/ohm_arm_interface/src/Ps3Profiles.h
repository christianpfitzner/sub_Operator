/*
 * Ps4Profiles.h
 *
 *  Created on: Apr 10, 2018
 *      Author: phil
 */

#ifndef OHM_SCHR_REMOTE_SRC_MAPPERS_PS3PROFILES_H_
#define OHM_SCHR_REMOTE_SRC_MAPPERS_PS3PROFILES_H_

//#define OLD

#ifdef OLD
enum Axis_ps3_sixad
 {
   A1_X = 0,         //!< y axis of left analog stick
   A1_Y,             //!< x axis of left analog stick
   A2_X,             //!< y axis of right analog stick
   A2_Y,             //!< x axis of right analog stick
   SIXXAXIS_Y,       //Accelerometer
   SIXXAXIS_X,       //Accelerometer
   SIXXAXIS_Z,       //Accelerometer
   nc,
   CROSS_UP,         //!< command cross up
   CROSS_R,          //!< command cross right
   CROSS_D,          //!< command cross down
   CROSS_L,          //!< command cross left
   L2,               //!< L2
   R2,               //!< R2
   L1,               //!< L1
   R1,               //!< R1
   AB_T,             //!< triangle button
   AB_C,             //!< circle button
   AB_X,             //!< cross button
   AB_S,             //!< square botton
 };

 /**
  *  @enum Buttons_ps3
  *  Buttons of the ps3 controller
  */
 enum Buttons_ps3_sixad
 {
   B_SELECT = 0,     //!< SELECT
   B_A1,             //!< A1
   B_A2,             //!< A2
   B_START,          //!< START
   B_UP,             //!< UP
   B_RIGHT,          //!< RIGHT
   B_DOWN,           //!< DOWN
   B_LEFT,           //!< LEFT
   B_L2,             //!< L2
   B_R2,             //!< R2
   B_L1,             //!< L1
   B_R1,             //!< R1
   B_T,              //!< triangle button
   B_C,              //!< circle button
   B_X,              //!< cross button
   B_S,              //!< square button
   B_PS              //!< PS button
 };
#else
 enum Axis_ps3_sixad
  {
    A1_X = 0,         //!< x axis of left analog stick
    A1_Y,             //!< y axis of left analog stick
    L2,               //!< y axis of right analog stick
    A2_X,             //!< x axis of right analog stick
    A2_Y,               //!< L2
    R2,               //!< R2
  };

  /**
   *  @enum Buttons_ps3
   *  Buttons of the ps3 controller
   */
  enum Buttons_ps3_sixad
  {
    B_X = 0,     //!< SELECT
    B_C,             //!< A1
    B_T,             //!< A2
    B_S,          //!< START
    B_L1,             //!< UP
    B_R1,          //!< RIGHT
    B_L2,           //!< DOWN
    B_R2,           //!< LEFT
    B_SELECT,             //!< L2
    B_START,             //!< R2
    B_PS,             //!< L1
    B_A1,             //!< A1
    B_A2,             //!< A2
    B_UP,             //!< UP
    B_DOWN,          //!< RIGHT
    B_LEFT,           //!< DOWN
    B_RIGHT           //!< LEFT
  };
#endif
#endif /* OHM_SCHR_REMOTE_SRC_MAPPERS_PS3PROFILES_H_ */

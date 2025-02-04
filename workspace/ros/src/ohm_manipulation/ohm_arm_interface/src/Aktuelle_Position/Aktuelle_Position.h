/*
 * File: Aktuelle_Position.h
 *
 * MATLAB Coder version            : 2.6
 * C/C++ source code generated on  : 28-Apr-2018 09:19:58
 */

#ifndef __AKTUELLE_POSITION_H__
#define __AKTUELLE_POSITION_H__

/* Include files */
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "Aktuelle_Position_types.h"

/* Function Declarations */
extern void Aktuelle_Position(double thetaold[7], const double rd[3], double pd[3], double kord, double rd_3x3[9], const double spaceConstraint = 1.22);
extern void Aktuelle_Position_initialize(void);
extern void Aktuelle_Position_terminate(void);

#endif

/*
 * File trailer for Aktuelle_Position.h
 *
 * [EOF]
 */

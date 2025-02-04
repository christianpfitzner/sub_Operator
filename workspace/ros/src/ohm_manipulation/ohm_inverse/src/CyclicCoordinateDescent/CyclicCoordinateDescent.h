/*
 * File: CyclicCoordinateDescent.h
 *
 * MATLAB Coder version            : 2.6
 * C/C++ source code generated on  : 15-Mar-2018 13:40:53
 */

#ifndef __CYCLICCOORDINATEDESCENT_H__
#define __CYCLICCOORDINATEDESCENT_H__

/* Include files */
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include "rt_defines.h"
#include "rt_nonfinite.h"
#include "rtwtypes.h"
#include "CyclicCoordinateDescent_types.h"

/* Function Declarations */
extern void CyclicCoordinateDescent(const double a[4], const double alpha[4],
  const double d[4], const double Rd[9], const double pd[3], double theta[7],
  const double wo[3], const double alp, const double Epsilon, double output[8]);
extern void CyclicCoordinateDescent_initialize(void);
extern void CyclicCoordinateDescent_terminate(void);

#endif

/*
 * File trailer for CyclicCoordinateDescent.h
 *
 * [EOF]
 */


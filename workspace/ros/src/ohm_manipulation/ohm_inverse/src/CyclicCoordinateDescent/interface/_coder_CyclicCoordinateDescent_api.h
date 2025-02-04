/* 
 * File: _coder_CyclicCoordinateDescent_api.h 
 *  
 * MATLAB Coder version            : 2.6 
 * C/C++ source code generated on  : 31-Jul-2018 15:24:51 
 */

#ifndef ___CODER_CYCLICCOORDINATEDESCENT_API_H__
#define ___CODER_CYCLICCOORDINATEDESCENT_API_H__
/* Include files */
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "tmwtypes.h"
#include "mex.h"
#include "emlrt.h"

/* Function Declarations */
extern void CyclicCoordinateDescent_initialize(emlrtContext *aContext);
extern void CyclicCoordinateDescent_terminate(void);
extern void CyclicCoordinateDescent_atexit(void);
extern void CyclicCoordinateDescent_api(const mxArray *prhs[9], const mxArray *plhs[1]);
extern void CyclicCoordinateDescent(double a[4], double alpha[4], double d[4], double Rd[9], double pd[3], double theta[7], double wo[3], double alp, double Epsilon, double output[8]);
extern void CyclicCoordinateDescent_xil_terminate(void);

#endif
/* 
 * File trailer for _coder_CyclicCoordinateDescent_api.h 
 *  
 * [EOF] 
 */

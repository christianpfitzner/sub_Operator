/* 
 * File: _coder_Aktuelle_Position_api.h 
 *  
 * MATLAB Coder version            : 2.6 
 * C/C++ source code generated on  : 28-Apr-2018 09:19:58 
 */

#ifndef ___CODER_AKTUELLE_POSITION_API_H__
#define ___CODER_AKTUELLE_POSITION_API_H__
/* Include files */
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "tmwtypes.h"
#include "mex.h"
#include "emlrt.h"

/* Function Declarations */
extern void Aktuelle_Position_initialize(emlrtContext *aContext);
extern void Aktuelle_Position_terminate(void);
extern void Aktuelle_Position_atexit(void);
extern void Aktuelle_Position_api(const mxArray *prhs[4], const mxArray *plhs[2]);
extern void Aktuelle_Position_xil_terminate(void);
extern void Aktuelle_Position(double thetaold[7], double rd[3], double pd[3], double kord, double rd_3x3[9]);

#endif
/* 
 * File trailer for _coder_Aktuelle_Position_api.h 
 *  
 * [EOF] 
 */

/*
 * File: _coder_Aktuelle_Position_api.c
 *
 * MATLAB Coder version            : 2.6
 * C/C++ source code generated on  : 28-Apr-2018 09:19:58
 */

/* Include files */
#include "_coder_Aktuelle_Position_api.h"

/* Function Declarations */
static double (*emlrt_marshallIn(const emlrtStack *sp, const mxArray *thetaold,
  const char *identifier))[7];
static double (*b_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId))[7];
static double (*c_emlrt_marshallIn(const emlrtStack *sp, const mxArray *rd,
  const char *identifier))[3];
static double (*d_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId))[3];
static double e_emlrt_marshallIn(const emlrtStack *sp, const mxArray *kord,
  const char *identifier);
static double f_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId);
static void emlrt_marshallOut(const double u[3], const mxArray *y);
static const mxArray *b_emlrt_marshallOut(const double u[9]);
static double (*g_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
  const emlrtMsgIdentifier *msgId))[7];
static double (*h_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
  const emlrtMsgIdentifier *msgId))[3];
static double i_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src, const
  emlrtMsgIdentifier *msgId);

/* Function Definitions */

/*
 * Arguments    : emlrtContext *aContext
 * Return Type  : void
 */
void Aktuelle_Position_initialize(emlrtContext *aContext)
{
  emlrtStack st = { NULL, NULL, NULL };

  emlrtCreateRootTLS(&emlrtRootTLSGlobal, aContext, NULL, 1);
  st.tls = emlrtRootTLSGlobal;
  emlrtClearAllocCountR2012b(&st, false, 0U, 0);
  emlrtEnterRtStackR2012b(&st);
  emlrtFirstTimeR2012b(emlrtRootTLSGlobal);
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void Aktuelle_Position_terminate(void)
{
  emlrtStack st = { NULL, NULL, NULL };

  st.tls = emlrtRootTLSGlobal;
  emlrtLeaveRtStackR2012b(&st);
  emlrtDestroyRootTLS(&emlrtRootTLSGlobal);
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void Aktuelle_Position_atexit(void)
{
  emlrtStack st = { NULL, NULL, NULL };

  emlrtCreateRootTLS(&emlrtRootTLSGlobal, &emlrtContextGlobal, NULL, 1);
  st.tls = emlrtRootTLSGlobal;
  emlrtEnterRtStackR2012b(&st);
  emlrtLeaveRtStackR2012b(&st);
  emlrtDestroyRootTLS(&emlrtRootTLSGlobal);
  Aktuelle_Position_xil_terminate();
}

/*
 * Arguments    : const mxArray *prhs[4]
 *                const mxArray *plhs[2]
 * Return Type  : void
 */
void Aktuelle_Position_api(const mxArray *prhs[4], const mxArray *plhs[2])
{
  double (*rd_3x3)[9];
  double (*thetaold)[7];
  double (*rd)[3];
  double (*pd)[3];
  double kord;
  emlrtStack st = { NULL, NULL, NULL };

  st.tls = emlrtRootTLSGlobal;
  rd_3x3 = (double (*)[9])mxMalloc(sizeof(double [9]));
  prhs[0] = emlrtProtectR2012b(prhs[0], 0, false, -1);
  prhs[1] = emlrtProtectR2012b(prhs[1], 1, false, -1);
  prhs[2] = emlrtProtectR2012b(prhs[2], 2, true, -1);

  /* Marshall function inputs */
  thetaold = emlrt_marshallIn(&st, emlrtAlias(prhs[0]), "thetaold");
  rd = c_emlrt_marshallIn(&st, emlrtAlias(prhs[1]), "rd");
  pd = c_emlrt_marshallIn(&st, emlrtAlias(prhs[2]), "pd");
  kord = e_emlrt_marshallIn(&st, emlrtAliasP(prhs[3]), "kord");

  /* Invoke the target function */
  Aktuelle_Position(*thetaold, *rd, *pd, kord, *rd_3x3);

  /* Marshall function outputs */
  emlrt_marshallOut(*pd, prhs[2]);
  plhs[0] = prhs[2];
  plhs[1] = b_emlrt_marshallOut(*rd_3x3);
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *thetaold
 *                const char *identifier
 * Return Type  : double (*)[7]
 */
static double (*emlrt_marshallIn(const emlrtStack *sp, const mxArray *thetaold,
  const char *identifier))[7]
{
  double (*y)[7];
  emlrtMsgIdentifier thisId;
  thisId.fIdentifier = identifier;
  thisId.fParent = NULL;
  y = b_emlrt_marshallIn(sp, emlrtAlias(thetaold), &thisId);
  emlrtDestroyArray(&thetaold);
  return y;
}
/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *u
 *                const emlrtMsgIdentifier *parentId
 * Return Type  : double (*)[7]
 */
  static double (*b_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u,
  const emlrtMsgIdentifier *parentId))[7]
{
  double (*y)[7];
  y = g_emlrt_marshallIn(sp, emlrtAlias(u), parentId);
  emlrtDestroyArray(&u);
  return y;
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *rd
 *                const char *identifier
 * Return Type  : double (*)[3]
 */
static double (*c_emlrt_marshallIn(const emlrtStack *sp, const mxArray *rd,
  const char *identifier))[3]
{
  double (*y)[3];
  emlrtMsgIdentifier thisId;
  thisId.fIdentifier = identifier;
  thisId.fParent = NULL;
  y = d_emlrt_marshallIn(sp, emlrtAlias(rd), &thisId);
  emlrtDestroyArray(&rd);
  return y;
}
/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *u
 *                const emlrtMsgIdentifier *parentId
 * Return Type  : double (*)[3]
 */
  static double (*d_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u,
  const emlrtMsgIdentifier *parentId))[3]
{
  double (*y)[3];
  y = h_emlrt_marshallIn(sp, emlrtAlias(u), parentId);
  emlrtDestroyArray(&u);
  return y;
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *kord
 *                const char *identifier
 * Return Type  : double
 */
static double e_emlrt_marshallIn(const emlrtStack *sp, const mxArray *kord,
  const char *identifier)
{
  double y;
  emlrtMsgIdentifier thisId;
  thisId.fIdentifier = identifier;
  thisId.fParent = NULL;
  y = f_emlrt_marshallIn(sp, emlrtAlias(kord), &thisId);
  emlrtDestroyArray(&kord);
  return y;
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *u
 *                const emlrtMsgIdentifier *parentId
 * Return Type  : double
 */
static double f_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId)
{
  double y;
  y = i_emlrt_marshallIn(sp, emlrtAlias(u), parentId);
  emlrtDestroyArray(&u);
  return y;
}

/*
 * Arguments    : const double u[3]
 *                const mxArray *y
 * Return Type  : void
 */
static void emlrt_marshallOut(const double u[3], const mxArray *y)
{
  static const int iv0[1] = { 3 };

  mxSetData((mxArray *)y, (void *)u);
  emlrtSetDimensions((mxArray *)y, iv0, 1);
}

/*
 * Arguments    : const double u[9]
 * Return Type  : const mxArray *
 */
static const mxArray *b_emlrt_marshallOut(const double u[9])
{
  const mxArray *y;
  static const int iv1[2] = { 0, 0 };

  const mxArray *m0;
  static const int iv2[2] = { 3, 3 };

  y = NULL;
  m0 = emlrtCreateNumericArray(2, iv1, mxDOUBLE_CLASS, mxREAL);
  mxSetData((mxArray *)m0, (void *)u);
  emlrtSetDimensions((mxArray *)m0, iv2, 2);
  emlrtAssign(&y, m0);
  return y;
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *src
 *                const emlrtMsgIdentifier *msgId
 * Return Type  : double (*)[7]
 */
static double (*g_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
  const emlrtMsgIdentifier *msgId))[7]
{
  double (*ret)[7];
  int iv3[2];
  int i0;
  for (i0 = 0; i0 < 2; i0++) {
    iv3[i0] = 1 + 6 * i0;
  }

  emlrtCheckBuiltInR2012b(sp, msgId, src, "double", false, 2U, iv3);
  ret = (double (*)[7])mxGetData(src);
  emlrtDestroyArray(&src);
  return ret;
}
/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *src
 *                const emlrtMsgIdentifier *msgId
 * Return Type  : double (*)[3]
 */
  static double (*h_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
  const emlrtMsgIdentifier *msgId))[3]
{
  double (*ret)[3];
  int iv4[1];
  iv4[0] = 3;
  emlrtCheckBuiltInR2012b(sp, msgId, src, "double", false, 1U, iv4);
  ret = (double (*)[3])mxGetData(src);
  emlrtDestroyArray(&src);
  return ret;
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *src
 *                const emlrtMsgIdentifier *msgId
 * Return Type  : double
 */
static double i_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src, const
  emlrtMsgIdentifier *msgId)
{
  double ret;
  emlrtCheckBuiltInR2012b(sp, msgId, src, "double", false, 0U, 0);
  ret = *(double *)mxGetData(src);
  emlrtDestroyArray(&src);
  return ret;
}

/*
 * File trailer for _coder_Aktuelle_Position_api.c
 *
 * [EOF]
 */

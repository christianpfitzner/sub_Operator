/*
 * File: _coder_CyclicCoordinateDescent_api.c
 *
 * MATLAB Coder version            : 2.6
 * C/C++ source code generated on  : 31-Jul-2018 15:24:51
 */

/* Include files */
#include "_coder_CyclicCoordinateDescent_api.h"

/* Function Declarations */
static double (*emlrt_marshallIn(const emlrtStack *sp, const mxArray *a, const
  char *identifier))[4];
static double (*b_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId))[4];
static double (*c_emlrt_marshallIn(const emlrtStack *sp, const mxArray *Rd,
  const char *identifier))[9];
static double (*d_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId))[9];
static double (*e_emlrt_marshallIn(const emlrtStack *sp, const mxArray *pd,
  const char *identifier))[3];
static double (*f_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId))[3];
static double (*g_emlrt_marshallIn(const emlrtStack *sp, const mxArray *theta,
  const char *identifier))[7];
static double (*h_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId))[7];
static double (*i_emlrt_marshallIn(const emlrtStack *sp, const mxArray *wo,
  const char *identifier))[3];
static double (*j_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId))[3];
static double k_emlrt_marshallIn(const emlrtStack *sp, const mxArray *alp, const
  char *identifier);
static double l_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId);
static const mxArray *emlrt_marshallOut(const double u[8]);
static double (*m_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
  const emlrtMsgIdentifier *msgId))[4];
static double (*n_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
  const emlrtMsgIdentifier *msgId))[9];
static double (*o_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
  const emlrtMsgIdentifier *msgId))[3];
static double (*p_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
  const emlrtMsgIdentifier *msgId))[7];
static double (*q_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
  const emlrtMsgIdentifier *msgId))[3];
static double r_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src, const
  emlrtMsgIdentifier *msgId);

/* Function Definitions */

/*
 * Arguments    : emlrtContext *aContext
 * Return Type  : void
 */
void CyclicCoordinateDescent_initialize(emlrtContext *aContext)
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
void CyclicCoordinateDescent_terminate(void)
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
void CyclicCoordinateDescent_atexit(void)
{
  emlrtStack st = { NULL, NULL, NULL };

  emlrtCreateRootTLS(&emlrtRootTLSGlobal, &emlrtContextGlobal, NULL, 1);
  st.tls = emlrtRootTLSGlobal;
  emlrtEnterRtStackR2012b(&st);
  emlrtLeaveRtStackR2012b(&st);
  emlrtDestroyRootTLS(&emlrtRootTLSGlobal);
  CyclicCoordinateDescent_xil_terminate();
}

/*
 * Arguments    : const mxArray *prhs[9]
 *                const mxArray *plhs[1]
 * Return Type  : void
 */
void CyclicCoordinateDescent_api(const mxArray *prhs[9], const mxArray *plhs[1])
{
  double (*output)[8];
  double (*a)[4];
  double (*alpha)[4];
  double (*d)[4];
  double (*Rd)[9];
  double (*pd)[3];
  double (*theta)[7];
  double (*wo)[3];
  double alp;
  double Epsilon;
  emlrtStack st = { NULL, NULL, NULL };

  st.tls = emlrtRootTLSGlobal;
  output = (double (*)[8])mxMalloc(sizeof(double [8]));
  prhs[0] = emlrtProtectR2012b(prhs[0], 0, false, -1);
  prhs[1] = emlrtProtectR2012b(prhs[1], 1, false, -1);
  prhs[2] = emlrtProtectR2012b(prhs[2], 2, false, -1);
  prhs[3] = emlrtProtectR2012b(prhs[3], 3, false, -1);
  prhs[4] = emlrtProtectR2012b(prhs[4], 4, false, -1);
  prhs[5] = emlrtProtectR2012b(prhs[5], 5, false, -1);
  prhs[6] = emlrtProtectR2012b(prhs[6], 6, false, -1);

  /* Marshall function inputs */
  a = emlrt_marshallIn(&st, emlrtAlias(prhs[0]), "a");
  alpha = emlrt_marshallIn(&st, emlrtAlias(prhs[1]), "alpha");
  d = emlrt_marshallIn(&st, emlrtAlias(prhs[2]), "d");
  Rd = c_emlrt_marshallIn(&st, emlrtAlias(prhs[3]), "Rd");
  pd = e_emlrt_marshallIn(&st, emlrtAlias(prhs[4]), "pd");
  theta = g_emlrt_marshallIn(&st, emlrtAlias(prhs[5]), "theta");
  wo = i_emlrt_marshallIn(&st, emlrtAlias(prhs[6]), "wo");
  alp = k_emlrt_marshallIn(&st, emlrtAliasP(prhs[7]), "alp");
  Epsilon = k_emlrt_marshallIn(&st, emlrtAliasP(prhs[8]), "Epsilon");

  /* Invoke the target function */
  CyclicCoordinateDescent(*a, *alpha, *d, *Rd, *pd, *theta, *wo, alp, Epsilon,
    *output);

  /* Marshall function outputs */
  plhs[0] = emlrt_marshallOut(*output);
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *a
 *                const char *identifier
 * Return Type  : double (*)[4]
 */
static double (*emlrt_marshallIn(const emlrtStack *sp, const mxArray *a, const
  char *identifier))[4]
{
  double (*y)[4];
  emlrtMsgIdentifier thisId;
  thisId.fIdentifier = identifier;
  thisId.fParent = NULL;
  y = b_emlrt_marshallIn(sp, emlrtAlias(a), &thisId);
  emlrtDestroyArray(&a);
  return y;
}
/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *u
 *                const emlrtMsgIdentifier *parentId
 * Return Type  : double (*)[4]
 */
  static double (*b_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u,
  const emlrtMsgIdentifier *parentId))[4]
{
  double (*y)[4];
  y = m_emlrt_marshallIn(sp, emlrtAlias(u), parentId);
  emlrtDestroyArray(&u);
  return y;
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *Rd
 *                const char *identifier
 * Return Type  : double (*)[9]
 */
static double (*c_emlrt_marshallIn(const emlrtStack *sp, const mxArray *Rd,
  const char *identifier))[9]
{
  double (*y)[9];
  emlrtMsgIdentifier thisId;
  thisId.fIdentifier = identifier;
  thisId.fParent = NULL;
  y = d_emlrt_marshallIn(sp, emlrtAlias(Rd), &thisId);
  emlrtDestroyArray(&Rd);
  return y;
}
/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *u
 *                const emlrtMsgIdentifier *parentId
 * Return Type  : double (*)[9]
 */
  static double (*d_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u,
  const emlrtMsgIdentifier *parentId))[9]
{
  double (*y)[9];
  y = n_emlrt_marshallIn(sp, emlrtAlias(u), parentId);
  emlrtDestroyArray(&u);
  return y;
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *pd
 *                const char *identifier
 * Return Type  : double (*)[3]
 */
static double (*e_emlrt_marshallIn(const emlrtStack *sp, const mxArray *pd,
  const char *identifier))[3]
{
  double (*y)[3];
  emlrtMsgIdentifier thisId;
  thisId.fIdentifier = identifier;
  thisId.fParent = NULL;
  y = f_emlrt_marshallIn(sp, emlrtAlias(pd), &thisId);
  emlrtDestroyArray(&pd);
  return y;
}
/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *u
 *                const emlrtMsgIdentifier *parentId
 * Return Type  : double (*)[3]
 */
  static double (*f_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u,
  const emlrtMsgIdentifier *parentId))[3]
{
  double (*y)[3];
  y = o_emlrt_marshallIn(sp, emlrtAlias(u), parentId);
  emlrtDestroyArray(&u);
  return y;
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *theta
 *                const char *identifier
 * Return Type  : double (*)[7]
 */
static double (*g_emlrt_marshallIn(const emlrtStack *sp, const mxArray *theta,
  const char *identifier))[7]
{
  double (*y)[7];
  emlrtMsgIdentifier thisId;
  thisId.fIdentifier = identifier;
  thisId.fParent = NULL;
  y = h_emlrt_marshallIn(sp, emlrtAlias(theta), &thisId);
  emlrtDestroyArray(&theta);
  return y;
}
/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *u
 *                const emlrtMsgIdentifier *parentId
 * Return Type  : double (*)[7]
 */
  static double (*h_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u,
  const emlrtMsgIdentifier *parentId))[7]
{
  double (*y)[7];
  y = p_emlrt_marshallIn(sp, emlrtAlias(u), parentId);
  emlrtDestroyArray(&u);
  return y;
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *wo
 *                const char *identifier
 * Return Type  : double (*)[3]
 */
static double (*i_emlrt_marshallIn(const emlrtStack *sp, const mxArray *wo,
  const char *identifier))[3]
{
  double (*y)[3];
  emlrtMsgIdentifier thisId;
  thisId.fIdentifier = identifier;
  thisId.fParent = NULL;
  y = j_emlrt_marshallIn(sp, emlrtAlias(wo), &thisId);
  emlrtDestroyArray(&wo);
  return y;
}
/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *u
 *                const emlrtMsgIdentifier *parentId
 * Return Type  : double (*)[3]
 */
  static double (*j_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u,
  const emlrtMsgIdentifier *parentId))[3]
{
  double (*y)[3];
  y = q_emlrt_marshallIn(sp, emlrtAlias(u), parentId);
  emlrtDestroyArray(&u);
  return y;
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *alp
 *                const char *identifier
 * Return Type  : double
 */
static double k_emlrt_marshallIn(const emlrtStack *sp, const mxArray *alp, const
  char *identifier)
{
  double y;
  emlrtMsgIdentifier thisId;
  thisId.fIdentifier = identifier;
  thisId.fParent = NULL;
  y = l_emlrt_marshallIn(sp, emlrtAlias(alp), &thisId);
  emlrtDestroyArray(&alp);
  return y;
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *u
 *                const emlrtMsgIdentifier *parentId
 * Return Type  : double
 */
static double l_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId)
{
  double y;
  y = r_emlrt_marshallIn(sp, emlrtAlias(u), parentId);
  emlrtDestroyArray(&u);
  return y;
}

/*
 * Arguments    : const double u[8]
 * Return Type  : const mxArray *
 */
static const mxArray *emlrt_marshallOut(const double u[8])
{
  const mxArray *y;
  static const int iv0[2] = { 0, 0 };

  const mxArray *m0;
  static const int iv1[2] = { 1, 8 };

  y = NULL;
  m0 = emlrtCreateNumericArray(2, iv0, mxDOUBLE_CLASS, mxREAL);
  mxSetData((mxArray *)m0, (void *)u);
  emlrtSetDimensions((mxArray *)m0, iv1, 2);
  emlrtAssign(&y, m0);
  return y;
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *src
 *                const emlrtMsgIdentifier *msgId
 * Return Type  : double (*)[4]
 */
static double (*m_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
  const emlrtMsgIdentifier *msgId))[4]
{
  double (*ret)[4];
  int iv2[2];
  int i0;
  for (i0 = 0; i0 < 2; i0++) {
    iv2[i0] = 1 + 3 * i0;
  }

  emlrtCheckBuiltInR2012b(sp, msgId, src, "double", false, 2U, iv2);
  ret = (double (*)[4])mxGetData(src);
  emlrtDestroyArray(&src);
  return ret;
}
/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *src
 *                const emlrtMsgIdentifier *msgId
 * Return Type  : double (*)[9]
 */
  static double (*n_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
  const emlrtMsgIdentifier *msgId))[9]
{
  double (*ret)[9];
  int iv3[2];
  int i;
  for (i = 0; i < 2; i++) {
    iv3[i] = 3;
  }

  emlrtCheckBuiltInR2012b(sp, msgId, src, "double", false, 2U, iv3);
  ret = (double (*)[9])mxGetData(src);
  emlrtDestroyArray(&src);
  return ret;
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *src
 *                const emlrtMsgIdentifier *msgId
 * Return Type  : double (*)[3]
 */
static double (*o_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
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
 * Return Type  : double (*)[7]
 */
  static double (*p_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
  const emlrtMsgIdentifier *msgId))[7]
{
  double (*ret)[7];
  int iv5[2];
  int i1;
  for (i1 = 0; i1 < 2; i1++) {
    iv5[i1] = 1 + 6 * i1;
  }

  emlrtCheckBuiltInR2012b(sp, msgId, src, "double", false, 2U, iv5);
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
static double (*q_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
  const emlrtMsgIdentifier *msgId))[3]
{
  double (*ret)[3];
  int iv6[2];
  int i2;
  for (i2 = 0; i2 < 2; i2++) {
    iv6[i2] = 1 + (i2 << 1);
  }

  emlrtCheckBuiltInR2012b(sp, msgId, src, "double", false, 2U, iv6);
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
  static double r_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
  const emlrtMsgIdentifier *msgId)
{
  double ret;
  emlrtCheckBuiltInR2012b(sp, msgId, src, "double", false, 0U, 0);
  ret = *(double *)mxGetData(src);
  emlrtDestroyArray(&src);
  return ret;
}

/*
 * File trailer for _coder_CyclicCoordinateDescent_api.c
 *
 * [EOF]
 */

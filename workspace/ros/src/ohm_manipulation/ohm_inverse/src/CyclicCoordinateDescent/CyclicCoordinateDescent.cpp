/*
 * File: CyclicCoordinateDescent.c
 *
 * MATLAB Coder version            : 2.6
 * C/C++ source code generated on  : 15-Mar-2018 13:40:53
 */

/* Include files */
#include "rt_nonfinite.h"
#include "CyclicCoordinateDescent.h"

/* Function Declarations */
static double dot(const double a[3], const double b[3]);
static double norm(const double x[3]);
static double rt_atan2d_snf(double u0, double u1);

/* Function Definitions */

/*
 * Arguments    : const double a[3]
 *                const double b[3]
 * Return Type  : double
 */
static double dot(const double a[3], const double b[3])
{
  double c;
  int ix;
  int iy;
  int k;
  c = 0.0;
  ix = 0;
  iy = 0;
  for (k = 0; k < 3; k++) {
    c += a[ix] * b[iy];
    ix++;
    iy++;
  }

  return c;
}

/*
 * Arguments    : const double x[3]
 * Return Type  : double
 */
static double norm(const double x[3])
{
  double y;
  double scale;
  int k;
  double absxk;
  double t;
  y = 0.0;
  scale = 2.2250738585072014E-308;
  for (k = 0; k < 3; k++) {
    absxk = fabs(x[k]);
    if (absxk > scale) {
      t = scale / absxk;
      y = 1.0 + y * t * t;
      scale = absxk;
    } else {
      t = absxk / scale;
      y += t * t;
    }
  }

  return scale * sqrt(y);
}

/*
 * Arguments    : double u0
 *                double u1
 * Return Type  : double
 */
static double rt_atan2d_snf(double u0, double u1)
{
  double y;
  int b_u0;
  int b_u1;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = rtNaN;
  } else if (rtIsInf(u0) && rtIsInf(u1)) {
    if (u0 > 0.0) {
      b_u0 = 1;
    } else {
      b_u0 = -1;
    }

    if (u1 > 0.0) {
      b_u1 = 1;
    } else {
      b_u1 = -1;
    }

    y = atan2(b_u0, b_u1);
  } else if (u1 == 0.0) {
    if (u0 > 0.0) {
      y = RT_PI / 2.0;
    } else if (u0 < 0.0) {
      y = -(double)(RT_PI / 2.0);
    } else {
      y = 0.0;
    }
  } else {
    y = atan2(u0, u1);
  }

  return y;
}

/*
 * Arguments    : const double a[4]
 *                const double alpha[4]
 *                const double d[4]
 *                const double Rd[9]
 *                const double pd[3]
 *                double theta[7]
 *                const double wo[3]
 *                double alp
 *                double Epsilon
 *                double output[8]
 * Return Type  : void
 */
void CyclicCoordinateDescent(const double a[4], const double alpha[4], const
  double d[4], const double Rd[9], const double pd[3], double theta[7], const
  double wo[3], const double alp, const double Epsilon, double output[8])
{
  double thetaalt[7];
  int ix;
  double iter;
  int32_T exitg1;
  double x[15];
  double y[15];
  double z[15];
  double p[15];
  static const signed char iv0[15] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0 };

  static const signed char iv1[15] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0 };

  static const signed char iv2[15] = { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0 };

  double pasterisk[12];
  int i;
  double deltap;
  double phiasterisk;
  double b_x[3];
  double phi[12];
  double pdi[12];
  int ixstart;
  double b_pd[3];
  double h1[3];
  double h2[3];
  double h3[3];
  boolean_T exitg3;
  double k1;
  boolean_T exitg2;
  double k2;
  double b_phi[3];
  double b_h1[3];
  double b_h2[3];
  double b_h3[3];
  double Rzphi[9];
  double b_Rzphi[3];
  static const double dv0[7] = { 7.2831853071795862, 2.42, 3.1415926535897931,
    1.5707963267948966, 3.2415926535897932, 1.5707963267948966,
    M_PI-0.1 };

  static const double dv1[7] = { -7.2831853071795862, 0.05, -1.5707963267948966,
    -3.1415926535897931, -3.2415926535897932, -1.5707963267948966,
    -M_PI+0.1 };

  /*  Function for CCD Iteration */
  /* 'CyclicCoordinateDescent:3' d1=Rd(1:3,1); */
  /* 'CyclicCoordinateDescent:4' d2=Rd(1:3,2); */
  /* 'CyclicCoordinateDescent:5' d3=Rd(1:3,3); */
  /* 'CyclicCoordinateDescent:6' thetaalt=zeros(1,7); */
  /* 'CyclicCoordinateDescent:7' thetaalt(1:7)=theta(1:7); */
  for (ix = 0; ix < 7; ix++) {
    thetaalt[ix] = theta[ix];

    /* 'CyclicCoordinateDescent:8' theta=zeros(1,7); */
    theta[ix] = 0.0;
  }

  /* 'CyclicCoordinateDescent:9' theta(1:4)=thetaalt(1:4); */
  for (ix = 0; ix < 4; ix++) {
    theta[ix] = thetaalt[ix];
  }

  /* 'CyclicCoordinateDescent:10' n=length(a); */
  /* 'CyclicCoordinateDescent:11' iter=1; */
  iter = 1.0;

  /* 'CyclicCoordinateDescent:13' while 1 */
  do {
    exitg1 = 0;

    /* 'CyclicCoordinateDescent:15' [x,y,z,pasterisk,jp]=ForwardRecursion(a,alpha,d,theta); */
    /*  Forward Recursion formulas by L.T.Wang and B.Ravani to calculate the $/ */
    /*  forward kinematics of a manipulation arm. $/ */
    /*  a = link length $/ */
    /*  alpha = twist angle $/ */
    /*  d = link offset $/ */
    /*  theta = joint variable $/ */
    /*  Function Forward Recursion */
    /* 'ForwardRecursion:10' n=length(a); */
    /* 'ForwardRecursion:11' x=[[1 0 0]',zeros(3,n)]; */
    for (ix = 0; ix < 15; ix++) {
      x[ix] = iv0[ix];

      /* 'ForwardRecursion:12' y=[[0 1 0]',zeros(3,n)]; */
      y[ix] = iv1[ix];

      /* 'ForwardRecursion:13' z=[[0 0 1]',zeros(3,n)]; */
      z[ix] = iv2[ix];

      /* 'ForwardRecursion:14' pasterisk=zeros(3,n); */
      /* 'ForwardRecursion:15' jp=zeros(3,n+1); */
      p[ix] = 0.0;
    }

    /* 'ForwardRecursion:16' for i=1:n */
    for (i = 0; i < 4; i++) {
      /* 'ForwardRecursion:18' x(1:3,i+1)=x(1:3,i)*cos(theta(i))+y(1:3,i)*sin(theta(i)); */
      deltap = cos(theta[i]);
      phiasterisk = sin(theta[i]);
      for (ix = 0; ix < 3; ix++) {
        x[ix + 3 * (i + 1)] = x[ix + 3 * i] * deltap + y[ix + 3 * i] *
          phiasterisk;
      }

      /* 'ForwardRecursion:19' z(1:3,i+1)=z(1:3,i)*cos(alpha(i))+cross(x(1:3,i+1),z(1:3,i))*sin(alpha(i)); */
      deltap = cos(alpha[i]);
      phiasterisk = sin(alpha[i]);
      b_x[0] = x[1 + 3 * (i + 1)] * z[2 + 3 * i] - x[2 + 3 * (i + 1)] * z[1 + 3 *
        i];
      b_x[1] = x[2 + 3 * (i + 1)] * z[3 * i] - x[3 * (i + 1)] * z[2 + 3 * i];
      b_x[2] = x[3 * (i + 1)] * z[1 + 3 * i] - x[1 + 3 * (i + 1)] * z[3 * i];
      for (ix = 0; ix < 3; ix++) {
        z[ix + 3 * (i + 1)] = z[ix + 3 * i] * deltap + b_x[ix] * phiasterisk;
      }

      /* 'ForwardRecursion:20' y(1:3,i+1)=cross(z(1:3,i+1),x(1:3,i+1)); */
      y[3 * (1 + i)] = z[1 + 3 * (i + 1)] * x[2 + 3 * (i + 1)] - z[2 + 3 * (i +
        1)] * x[1 + 3 * (i + 1)];
      y[1 + 3 * (1 + i)] = z[2 + 3 * (i + 1)] * x[3 * (i + 1)] - z[3 * (i + 1)] *
        x[2 + 3 * (i + 1)];
      y[2 + 3 * (1 + i)] = z[3 * (i + 1)] * x[1 + 3 * (i + 1)] - z[1 + 3 * (i +
        1)] * x[3 * (i + 1)];

      /* 'ForwardRecursion:22' pasterisk(1:3,i)=d(i)*z(1:3,i)+a(i)*x(1:3,i+1); */
      /* 'ForwardRecursion:24' jp(1:3,i+1)=jp(1:3,i)-pasterisk(1:3,i); */
      for (ix = 0; ix < 3; ix++) {
        pasterisk[ix + 3 * i] = d[i] * z[ix + 3 * i] + a[i] * x[ix + 3 * (i + 1)];
        p[ix + 3 * (i + 1)] = p[ix + 3 * i] - pasterisk[ix + 3 * i];
      }
    }

    /* call forward recursion function */
    /* 'CyclicCoordinateDescent:16' p=-jp; */
    for (ix = 0; ix < 15; ix++) {
      p[ix] = -p[ix];
    }

    /* 'CyclicCoordinateDescent:17' ph=p(1:3,n+1); */
    /* 'CyclicCoordinateDescent:18' phi=zeros(3,n); */
    /* 'CyclicCoordinateDescent:19' pdi=zeros(3,n); */
    /* 'CyclicCoordinateDescent:20' for c=1:n */
    for (ixstart = 0; ixstart < 4; ixstart++) {
      /* 'CyclicCoordinateDescent:21' phi(1:3,c)=ph(1:3,1)-p(1:3,c); */
      for (ix = 0; ix < 3; ix++) {
        phi[ix + 3 * ixstart] = p[12 + ix] - p[ix + 3 * ixstart];

        /* 'CyclicCoordinateDescent:22' pdi(1:3,c)=pd(1:3)-p(1:3,c); */
        pdi[ix + 3 * ixstart] = pd[ix] - p[ix + 3 * ixstart];
      }
    }

    /* 'CyclicCoordinateDescent:24' h1=x(1:3,n+1); */
    /* 'CyclicCoordinateDescent:27' deltap=norm(pd(1:3)-ph(1:3)); */
    for (ix = 0; ix < 3; ix++) {
      h1[ix] = x[12 + ix];

      /* 'CyclicCoordinateDescent:25' h2=y(1:3,n+1); */
      h2[ix] = y[12 + ix];

      /* 'CyclicCoordinateDescent:26' h3=z(1:3,n+1); */
      h3[ix] = z[12 + ix];
      b_pd[ix] = pd[ix] - p[12 + ix];
    }

    deltap = norm(b_pd);

    /* 'CyclicCoordinateDescent:28' Deltap=dot(deltap,deltap); */
    /*  Deltao=wo(1)*(dot(d1,h1)-1)^2+wo(2)*(dot(d2,h2)-1)^2+wo(3)*(dot(d3,h3)-1)^2; */
    /*  orientation error wont be considered, because of analytical solution of the orientation problem da */
    /* 'CyclicCoordinateDescent:33' Error=Deltap; */
    /* +Deltao; */
    /* 'CyclicCoordinateDescent:34' if Error<Epsilon */
    if (deltap * deltap < Epsilon) {
      exitg1 = 1;
    } else {
      /* 'CyclicCoordinateDescent:100' else */
      /*   if line 35 - Error<Epsilon */
      /* 'CyclicCoordinateDescent:101' for i=n:-1:1 */
      for (i = 0; i < 4; i++) {
        /* 'CyclicCoordinateDescent:102' wp=alp*(1+min([norm(pdi(1:3,i)),norm(phi(1:3,i))])/max([norm(pdi(1:3,i)),norm(phi(1:3,i))])); */
        phiasterisk = norm(*(double (*)[3])&pdi[3 * (3 - i)]);
        deltap = norm(*(double (*)[3])&phi[3 * (3 - i)]);
        ixstart = 1;
        if (rtIsNaN(phiasterisk)) {
          ix = 2;
          exitg3 = false;
          while ((!exitg3) && (ix < 3)) {
            ixstart = 2;
            if (!rtIsNaN(deltap)) {
              phiasterisk = deltap;
              exitg3 = true;
            } else {
              ix = 3;
            }
          }
        }

        if ((ixstart < 2) && (deltap < phiasterisk)) {
          phiasterisk = deltap;
        }

        k1 = norm(*(double (*)[3])&pdi[3 * (3 - i)]);
        deltap = norm(*(double (*)[3])&phi[3 * (3 - i)]);
        ixstart = 1;
        if (rtIsNaN(k1)) {
          ix = 2;
          exitg2 = false;
          while ((!exitg2) && (ix < 3)) {
            ixstart = 2;
            if (!rtIsNaN(deltap)) {
              k1 = deltap;
              exitg2 = true;
            } else {
              ix = 3;
            }
          }
        }

        if ((ixstart < 2) && (deltap > k1)) {
          k1 = deltap;
        }

        deltap = alp * (1.0 + phiasterisk / k1);

        /* 'CyclicCoordinateDescent:103' k1=wp*dot(pdi(1:3,i),z(1:3,i))*dot(phi(1:3,i),z(1:3,i))+wo(1)*dot(d1,z(1:3,i))*dot(h1,z(1:3,i))+wo(2)*dot(d2,z(1:3,i))*dot(h2,z(1:3,i))+wo(3)*dot(d3,z(1:3,i))*dot(h3,z(1:3,i)); */
        k1 = ((deltap * dot(*(double (*)[3])&pdi[3 * (3 - i)], *(double (*)[3])&
                            z[3 * (3 - i)]) * dot(*(double (*)[3])&phi[3 * (3 -
                 i)], *(double (*)[3])&z[3 * (3 - i)]) + wo[0] * dot(*(double (*)
                 [3])&Rd[0], *(double (*)[3])&z[3 * (3 - i)]) * dot(h1, *(double
                 (*)[3])&z[3 * (3 - i)])) + wo[1] * dot(*(double (*)[3])&Rd[3], *
               (double (*)[3])&z[3 * (3 - i)]) * dot(h2, *(double (*)[3])&z[3 *
               (3 - i)])) + wo[2] * dot(*(double (*)[3])&Rd[6], *(double (*)[3])
          &z[3 * (3 - i)]) * dot(h3, *(double (*)[3])&z[3 * (3 - i)]);

        /* 'CyclicCoordinateDescent:104' k2=wp*dot(pdi(1:3,i),phi(1:3,i))+wo(1)*dot(d1,h1)+wo(2)*dot(d2,h2)+wo(3)*dot(d3,h3); */
        k2 = ((deltap * dot(*(double (*)[3])&pdi[3 * (3 - i)], *(double (*)[3])&
                            phi[3 * (3 - i)]) + wo[0] * dot(*(double (*)[3])&Rd
                [0], h1)) + wo[1] * dot(*(double (*)[3])&Rd[3], h2)) + wo[2] *
          dot(*(double (*)[3])&Rd[6], h3);

        /* 'CyclicCoordinateDescent:105' k3=dot(z(1:3,i),wp*cross(phi(1:3,i),pdi(1:3,i))+wo(1)*cross(h1,d1)+wo(2)*cross(h2,d2)+wo(3)*cross(h3,d3)); */
        b_phi[0] = phi[1 + 3 * (3 - i)] * pdi[2 + 3 * (3 - i)] - phi[2 + 3 * (3
          - i)] * pdi[1 + 3 * (3 - i)];
        b_phi[1] = phi[2 + 3 * (3 - i)] * pdi[3 * (3 - i)] - phi[3 * (3 - i)] *
          pdi[2 + 3 * (3 - i)];
        b_phi[2] = phi[3 * (3 - i)] * pdi[1 + 3 * (3 - i)] - phi[1 + 3 * (3 - i)]
          * pdi[3 * (3 - i)];
        b_h1[0] = h1[1] * Rd[2] - h1[2] * Rd[1];
        b_h1[1] = h1[2] * Rd[0] - h1[0] * Rd[2];
        b_h1[2] = h1[0] * Rd[1] - h1[1] * Rd[0];
        b_h2[0] = h2[1] * Rd[5] - h2[2] * Rd[4];
        b_h2[1] = h2[2] * Rd[3] - h2[0] * Rd[5];
        b_h2[2] = h2[0] * Rd[4] - h2[1] * Rd[3];
        b_h3[0] = h3[1] * Rd[8] - h3[2] * Rd[7];
        b_h3[1] = h3[2] * Rd[6] - h3[0] * Rd[8];
        b_h3[2] = h3[0] * Rd[7] - h3[1] * Rd[6];
        for (ix = 0; ix < 3; ix++) {
          b_pd[ix] = ((deltap * b_phi[ix] + wo[0] * b_h1[ix]) + wo[1] * b_h2[ix])
            + wo[2] * b_h3[ix];
        }

        deltap = dot(*(double (*)[3])&z[3 * (3 - i)], b_pd);

        /* 'CyclicCoordinateDescent:106' phiasterisk=atan(-k3/(k1-k2)); */
        phiasterisk = ::atan(-deltap / (k1 - k2));

        /* 'CyclicCoordinateDescent:107' if (k1-k2)*cos(phiasterisk)-k3*sin(phiasterisk) >=0 */
        if ((k1 - k2) * cos(phiasterisk) - deltap * sin(phiasterisk) >= 0.0) {
          /* 'CyclicCoordinateDescent:108' phiasterisk=phiasterisk+pi; */
          phiasterisk += 3.1415926535897931;
        }

        /*  if (ki-k2)... */
        /* 'CyclicCoordinateDescent:113' theta(i)=theta(i)+phiasterisk; */
        theta[3 - i] += phiasterisk;

        /*  Check joint angles */
        /* 'CyclicCoordinateDescent:118' if theta(i)<=2*pi */
        if (theta[3 - i] <= 6.2831853071795862) {
          /* 'CyclicCoordinateDescent:119' if theta(i)>=-2*pi */
          if (theta[3 - i] >= -6.2831853071795862) {
            /* 'CyclicCoordinateDescent:120' theta(i)=theta(i); */
          } else {
            /* 'CyclicCoordinateDescent:121' else */
            /* 'CyclicCoordinateDescent:122' theta(i)=theta(i)+2*pi; */
            theta[3 - i] += 6.2831853071795862;
          }

          /* if theta(i)>=-2*pi */
        } else {
          /* 'CyclicCoordinateDescent:125' else */
          /* 'CyclicCoordinateDescent:126' theta(i)=theta(i)-2*pi; */
          theta[3 - i] -= 6.2831853071795862;
        }

        /* if theta(i)<=2*pi */
        /* 'CyclicCoordinateDescent:129' IK_CheckAngles(i,theta(i)); */
        /*  Function for checking Joint Limits */
        /* 'IK_CheckAngles:4' theta_u=[2*pi+1 ,2.42, pi, pi/2, pi+0.1, pi/2, pi+0.1]; */
        /* Upper Joint Limits */
        /* 'IK_CheckAngles:5' theta_l=[-2*pi-1, 0.05, -pi/2, -pi, -pi-0.1, -pi/2, -pi+0.1]; */
        /* Lower Joint Limits */
        /* 'IK_CheckAngles:7' if theta >= theta_u(joint) */
        /* check joint constraints / limits */
        /* 'CyclicCoordinateDescent:135' Rzphi=[z(1,i)^2*(1-cos(phiasterisk))+cos(phiasterisk) z(1,i)*z(2,i)*(1-cos(phiasterisk))-z(3,i)*sin(phiasterisk) z(1,i)*z(3,i)*(1-cos(phiasterisk))+z(2,i)*sin(phiasterisk); */
        /* 'CyclicCoordinateDescent:136'                     z(1,i)*z(2,i)*(1-cos(phiasterisk))+z(3,i)*sin(phiasterisk) z(2,i)^2*(1-cos(phiasterisk))+cos(phiasterisk) z(2,i)*z(3,i)*(1-cos(phiasterisk))-z(1,i)*sin(phiasterisk); */
        /* 'CyclicCoordinateDescent:137'                     z(1,i)*z(3,i)*(1-cos(phiasterisk))-z(2,i)*sin(phiasterisk) z(2,i)*z(3,i)*(1-cos(phiasterisk))+z(1,i)*sin(phiasterisk) z(3,i)^2*(1-cos(phiasterisk))+cos(phiasterisk)]; */
        Rzphi[0] = z[3 * (3 - i)] * z[3 * (3 - i)] * (1.0 - cos(phiasterisk)) +
          cos(phiasterisk);
        Rzphi[3] = z[3 * (3 - i)] * z[1 + 3 * (3 - i)] * (1.0 - cos(phiasterisk))
          - z[2 + 3 * (3 - i)] * sin(phiasterisk);
        Rzphi[6] = z[3 * (3 - i)] * z[2 + 3 * (3 - i)] * (1.0 - cos(phiasterisk))
          + z[1 + 3 * (3 - i)] * sin(phiasterisk);
        Rzphi[1] = z[3 * (3 - i)] * z[1 + 3 * (3 - i)] * (1.0 - cos(phiasterisk))
          + z[2 + 3 * (3 - i)] * sin(phiasterisk);
        Rzphi[4] = z[1 + 3 * (3 - i)] * z[1 + 3 * (3 - i)] * (1.0 - cos
          (phiasterisk)) + cos(phiasterisk);
        Rzphi[7] = z[1 + 3 * (3 - i)] * z[2 + 3 * (3 - i)] * (1.0 - cos
          (phiasterisk)) - z[3 * (3 - i)] * sin(phiasterisk);
        Rzphi[2] = z[3 * (3 - i)] * z[2 + 3 * (3 - i)] * (1.0 - cos(phiasterisk))
          - z[1 + 3 * (3 - i)] * sin(phiasterisk);
        Rzphi[5] = z[1 + 3 * (3 - i)] * z[2 + 3 * (3 - i)] * (1.0 - cos
          (phiasterisk)) + z[3 * (3 - i)] * sin(phiasterisk);
        Rzphi[8] = z[2 + 3 * (3 - i)] * z[2 + 3 * (3 - i)] * (1.0 - cos
          (phiasterisk)) + cos(phiasterisk);

        /* 'CyclicCoordinateDescent:138' phi(1:3,i)=Rzphi*phi(1:3,i); */
        for (ix = 0; ix < 3; ix++) {
          b_pd[ix] = 0.0;
          for (ixstart = 0; ixstart < 3; ixstart++) {
            b_pd[ix] += Rzphi[ix + 3 * ixstart] * phi[ixstart + 3 * (3 - i)];
          }
        }

        /* 'CyclicCoordinateDescent:139' h1=Rzphi*h1; */
        for (ix = 0; ix < 3; ix++) {
          phi[ix + 3 * (3 - i)] = b_pd[ix];
          b_Rzphi[ix] = 0.0;
          for (ixstart = 0; ixstart < 3; ixstart++) {
            b_Rzphi[ix] += Rzphi[ix + 3 * ixstart] * h1[ixstart];
          }
        }

        /* 'CyclicCoordinateDescent:140' h2=Rzphi*h2; */
        for (ix = 0; ix < 3; ix++) {
          h1[ix] = b_Rzphi[ix];
          b_pd[ix] = 0.0;
          for (ixstart = 0; ixstart < 3; ixstart++) {
            b_pd[ix] += Rzphi[ix + 3 * ixstart] * h2[ixstart];
          }
        }

        /* 'CyclicCoordinateDescent:141' h3=Rzphi*h3; */
        for (ix = 0; ix < 3; ix++) {
          h2[ix] = b_pd[ix];
          b_Rzphi[ix] = 0.0;
          for (ixstart = 0; ixstart < 3; ixstart++) {
            b_Rzphi[ix] += Rzphi[ix + 3 * ixstart] * h3[ixstart];
          }
        }

        for (ix = 0; ix < 3; ix++) {
          h3[ix] = b_Rzphi[ix];
        }

        /* 'CyclicCoordinateDescent:142' if i>1 */
        if (4 - i > 1) {
          /* 'CyclicCoordinateDescent:143' phi(1:3,i-1)=phi(1:3,i)+pasterisk(1:3,i-1); */
          for (ix = 0; ix < 3; ix++) {
            phi[ix + 3 * (2 - i)] = phi[ix + 3 * (3 - i)] + pasterisk[ix + 3 *
              (2 - i)];
          }
        }

        /*  if i>l */
      }

      /*  For-loop i=n:-1:1 */
      /* 'CyclicCoordinateDescent:146' iter=iter+1; */
      iter++;

      /* 'CyclicCoordinateDescent:147' if iter>30 */
      /*  if iter>30 */
      /*  if aus 35 */
    }
  } while (exitg1 == 0);

  /* 'CyclicCoordinateDescent:35' R_des=Rd; */
  /* 'CyclicCoordinateDescent:36' theta(2)=theta(2)-pi/2; */
  theta[1] -= 1.5707963267948966;

  /* 'CyclicCoordinateDescent:37' theta(2:4)=-theta(2:4); */
  for (ix = 0; ix < 3; ix++) {
    theta[1 + ix] = -theta[1 + ix];
  }

  /*  coordinate transformation to solve orientation problem */
  /*  calculate both atan2 for joint 5 and chose the angle which is closer to the */
  /*  old joint angle 5 - also change joint angle 6 */
  /* 'CyclicCoordinateDescent:42' theta5a=atan2(R_des(2,3)*cos(theta(1))-R_des(1,3)*sin(theta(1)),R_des(1,3)*cos(theta(2)+theta(3)+theta(4))*cos(theta(1))+R_des(2,3)*cos(theta(2)+theta(3)+theta(4))*sin(theta(1))-R_des(3,3)*sin(theta(2)+theta(3)+theta(4))); */
  deltap = rt_atan2d_snf(Rd[7] * cos(theta[0]) - Rd[6] * sin(theta[0]), (Rd[6] *
    cos((theta[1] + theta[2]) + theta[3]) * cos(theta[0]) + Rd[7] * cos((theta[1]
    + theta[2]) + theta[3]) * sin(theta[0])) - Rd[8] * sin((theta[1] + theta[2])
    + theta[3]));

  /* 'CyclicCoordinateDescent:43' theta5b=atan2(-(R_des(2,3)*cos(theta(1))-R_des(1,3)*sin(theta(1))),-(R_des(1,3)*cos(theta(2)+theta(3)+theta(4))*cos(theta(1))+R_des(2,3)*cos(theta(2)+theta(3)+theta(4))*sin(theta(1))-R_des(3,3)*sin(theta(2)+theta(3)+theta(4)))); */
  phiasterisk = rt_atan2d_snf(-(Rd[7] * cos(theta[0]) - Rd[6] * sin(theta[0])),
    -((Rd[6] * cos((theta[1] + theta[2]) + theta[3]) * cos(theta[0]) + Rd[7] *
       cos((theta[1] + theta[2]) + theta[3]) * sin(theta[0])) - Rd[8] * sin
      ((theta[1] + theta[2]) + theta[3])));

  /* 'CyclicCoordinateDescent:45' diff5a = abs(thetaalt(5) - theta5a); */
  /* 'CyclicCoordinateDescent:46' diff5b = abs(thetaalt(5) - theta5b); */
  /* 'CyclicCoordinateDescent:48' mp=R_des(1,3)*sin(theta(2)+theta(3)+theta(4))*cos(theta(1))+R_des(2,3)*sin(theta(2)+theta(3)+theta(4))*sin(theta(1))+R_des(3,3)*cos(theta(2)+theta(3)+theta(4)); */
  k1 = (Rd[6] * sin((theta[1] + theta[2]) + theta[3]) * cos(theta[0]) + Rd[7] *
        sin((theta[1] + theta[2]) + theta[3]) * sin(theta[0])) + Rd[8] * cos
    ((theta[1] + theta[2]) + theta[3]);

  /* 'CyclicCoordinateDescent:50' theta6a=atan2(sqrt(1-mp^2),mp); */
  /* 'CyclicCoordinateDescent:51' theta6b=atan2(-(sqrt(1-mp^2)),mp); */
  /* 'CyclicCoordinateDescent:53' if diff5a < diff5b */
  if (fabs(thetaalt[4] - deltap) < fabs(thetaalt[4] - phiasterisk)) {
    /* 'CyclicCoordinateDescent:54' theta(5)=theta5a; */
    theta[4] = deltap;

    /* 'CyclicCoordinateDescent:55' if mp^2>1 */
    if (k1 * k1 > 1.0) {
      /* 'CyclicCoordinateDescent:56' theta(6)=thetaalt(6); */
      theta[5] = thetaalt[5];
    } else {
      /* 'CyclicCoordinateDescent:57' else */
      /* 'CyclicCoordinateDescent:58' theta(6)=theta6a; */
      theta[5] = rt_atan2d_snf(sqrt(1.0 - k1 * k1), k1);
    }
  } else {
    /* 'CyclicCoordinateDescent:60' else */
    /* 'CyclicCoordinateDescent:61' theta(5)=theta5b; */
    theta[4] = phiasterisk;

    /* 'CyclicCoordinateDescent:62' if mp^2>1 */
    if (k1 * k1 > 1.0) {
      /* 'CyclicCoordinateDescent:63' theta(6)=thetaalt(6); */
      theta[5] = thetaalt[5];
    } else {
      /* 'CyclicCoordinateDescent:64' else */
      /* 'CyclicCoordinateDescent:65' theta(6)=theta6b; */
      theta[5] = rt_atan2d_snf(-sqrt(1.0 - k1 * k1), k1);
    }
  }

  /*  calculate both atan2 for joint 7 and chose the angle which is closer to the */
  /*  old joint angle 7  */
  /* 'CyclicCoordinateDescent:73' theta7a=atan2(R_des(1,2)*sin(theta(2)+theta(3)+theta(4))*cos(theta(1))+R_des(2,2)*sin(theta(2)+theta(3)+theta(4))*sin(theta(1))+R_des(3,2)*cos(theta(2)+theta(3)+theta(4)),-R_des(1,1)*sin(theta(2)+theta(3)+theta(4))*cos(theta(1))-R_des(2,1)*sin(theta(2)+theta(3)+theta(4))*sin(theta(1))-R_des(3,1)*cos(theta(2)+theta(3)+theta(4))) ; */
  deltap = rt_atan2d_snf((Rd[3] * sin((theta[1] + theta[2]) + theta[3]) * cos
    (theta[0]) + Rd[4] * sin((theta[1] + theta[2]) + theta[3]) * sin(theta[0]))
    + Rd[5] * cos((theta[1] + theta[2]) + theta[3]), (-Rd[0] * sin((theta[1] +
    theta[2]) + theta[3]) * cos(theta[0]) - Rd[1] * sin((theta[1] + theta[2]) +
    theta[3]) * sin(theta[0])) - Rd[2] * cos((theta[1] + theta[2]) + theta[3]));

  /* 'CyclicCoordinateDescent:74' theta7b=atan2(-(R_des(1,2)*sin(theta(2)+theta(3)+theta(4))*cos(theta(1))+R_des(2,2)*sin(theta(2)+theta(3)+theta(4))*sin(theta(1))+R_des(3,2)*cos(theta(2)+theta(3)+theta(4))),R_des(1,1)*sin(theta(2)+theta(3)+theta(4))*cos(theta(1))+R_des(2,1)*sin(theta(2)+theta(3)+theta(4))*sin(theta(1))+R_des(3,1)*cos(theta(2)+theta(3)+theta(4))); */
  phiasterisk = rt_atan2d_snf(-((Rd[3] * sin((theta[1] + theta[2]) + theta[3]) *
    cos(theta[0]) + Rd[4] * sin((theta[1] + theta[2]) + theta[3]) * sin(theta[0]))
    + Rd[5] * cos((theta[1] + theta[2]) + theta[3])), (Rd[0] * sin((theta[1] +
    theta[2]) + theta[3]) * cos(theta[0]) + Rd[1] * sin((theta[1] + theta[2]) +
    theta[3]) * sin(theta[0])) + Rd[2] * cos((theta[1] + theta[2]) + theta[3]));

  /* 'CyclicCoordinateDescent:76' diff7a = abs(thetaalt(7) - theta7a); */
  /* 'CyclicCoordinateDescent:77' diff7b = abs(thetaalt(7) - theta7b); */
  /* 'CyclicCoordinateDescent:79' if diff7a < diff7b */
  if (fabs(thetaalt[6] - deltap) < fabs(thetaalt[6] - phiasterisk)) {
    /* 'CyclicCoordinateDescent:80' theta(7)=theta7a; */
    theta[6] = deltap;
  } else {
    /* 'CyclicCoordinateDescent:81' else */
    /* 'CyclicCoordinateDescent:82' theta(7)=theta7b; */
    theta[6] = phiasterisk;
  }

  /* 'CyclicCoordinateDescent:85' theta(2:4)=-theta(2:4); */
  for (ix = 0; ix < 3; ix++) {
    theta[1 + ix] = -theta[1 + ix];
  }

  /* 'CyclicCoordinateDescent:86' theta(2)=theta(2)+pi/2; */
  theta[1] += 1.5707963267948966;

  /* 'CyclicCoordinateDescent:89' for i=5:7 */
  for (i = 0; i < 3; i++) {
    /* 'CyclicCoordinateDescent:90' theta(i)=IK_CheckAngles(i,theta(i)); */
    deltap = theta[i + 4];

    /*  Function for checking Joint Limits */
    /* 'IK_CheckAngles:4' theta_u=[2*pi+1 ,2.42, pi, pi/2, pi+0.1, pi/2, pi+0.1]; */
    /* Upper Joint Limits */
    /* 'IK_CheckAngles:5' theta_l=[-2*pi-1, 0.05, -pi/2, -pi, -pi-0.1, -pi/2, -pi+0.1]; */
    /* Lower Joint Limits */
    /* 'IK_CheckAngles:7' if theta >= theta_u(joint) */
    if (theta[i + 4] >= dv0[4 + i]) {
      /* 'IK_CheckAngles:8' theta = theta_u(joint); */
      deltap = dv0[4 + i];
    } else if (theta[i + 4] <= dv1[4 + i]) {
      /* 'IK_CheckAngles:9' elseif theta <= theta_l(joint) */
      /* 'IK_CheckAngles:10' theta = theta_l(joint); */
      deltap = dv1[4 + i];
    } else {
      /* 'IK_CheckAngles:11' else */
      /* 'IK_CheckAngles:12' theta = theta; */
    }

    theta[4 + i] = deltap;
  }

  /* 'CyclicCoordinateDescent:97' output=[theta,iter]; */
  for (ix = 0; ix < 7; ix++) {
    output[ix] = theta[ix];
  }

  output[7] = iter;

  /* disp(['Cyclic Coordinate Descent successful after ',num2str(iter) ,' iterations!']) */
  /* while */
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void CyclicCoordinateDescent_initialize(void)
{
  rt_InitInfAndNaN(8U);
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void CyclicCoordinateDescent_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for CyclicCoordinateDescent.c
 *
 * [EOF]
 */


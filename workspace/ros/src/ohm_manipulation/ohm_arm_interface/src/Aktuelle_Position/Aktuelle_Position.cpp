/*
 * File: Aktuelle_Position.c
 *
 * MATLAB Coder version            : 2.6
 * C/C++ source code generated on  : 28-Apr-2018 09:19:58
 */

/* Include files */
#include "rt_nonfinite.h"
#include "Aktuelle_Position.h"
#include <ros/ros.h>

/* Function Definitions */

/*
 * Arguments    : double thetaold[7]
 *                const double rd[3]
 *                double pd[3]
 *                double kord
 *                double rd_3x3[9]
 * Return Type  : void
 */
void Aktuelle_Position(double thetaold[7], const double rd[3], double pd[3], double kord, double rd_3x3[9], const double spaceConstraint)
{
  double pd_input[3];
  int i;
  double x[15];
  double y[15];
  double z[15];
  double jp[15];
  int i0;
  static const signed char iv0[15] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0 };

  static const signed char iv1[15] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0 };

  static const signed char iv2[15] = { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0 };

  double pasterisk[12];
  double b_x;
  double c_x;
  static const double dv0[4] = { 1.5707963267948966, 0.0, 0.0, 0.0 };

  double ph[3];
  static const double dv1[4] = { 0.0, 0.385, 0.458, 0.45 };

  double b_ph[3];
  double dv2[9];
  double dv3[9];
  static const signed char iv3[3] = { 0, 0, 1 };

  static const signed char iv4[3] = { 0, 1, 0 };

  double R07[9];
  double dv4[9];
  int i1;
  double b_R07[9];
  static const double b[3] = { 0.0, 0.0, 0.15 };

  static const signed char iv5[3] = { 1, 0, 0 };

  double b_rd_3x3[3];

  /*  function to calculate new position and orientation by considering the */
  /*  actual coordiante frame */
  /* 'Aktuelle_Position:5' pd_input = pd; */
  for (i = 0; i < 3; i++) {
    pd_input[i] = pd[i];
  }

  /* 'Aktuelle_Position:7' ph=zeros(3,1); */
  /* 'Aktuelle_Position:8' l_endeffektor = [0.0; 0.0; 0.15]; */
  /* length endeffector */
  /* %%%calculate with forward recursion formulars position till endeffector  %%%%%%%%%%%% */
  /* 'Aktuelle_Position:11' a = [0 0.385 0.458 0.450]; */
  /* 'Aktuelle_Position:12' alpha = [ 1.5707963267948966 0.0 0.0 0.0]; */
  /* 'Aktuelle_Position:13' d = [0 0 0 0 ]; */
  /* 'Aktuelle_Position:15' thetaold_pos = thetaold(1:4); */
  /* 'Aktuelle_Position:17' [x,y,z,pasterisk,jp] = ForwardRecursion(a,alpha,d,thetaold_pos); */
  /*  Forward Recursion formulas by L.T.Wang and B.Ravani to calculate the $/ */
  /*  forward kinematics of a manipulation arm. $/ */
  /*  a = link length $/ */
  /*  alpha = twist angle $/ */
  /*  d = link offset $/ */
  /*  theta = joint variable $/ */
  /*  Function Forward Recursion */
  /* 'ForwardRecursion:10' n=length(a); */
  /* 'ForwardRecursion:11' x=[[1 0 0]',zeros(3,n)]; */
  for (i0 = 0; i0 < 15; i0++) {
    x[i0] = iv0[i0];

    /* 'ForwardRecursion:12' y=[[0 1 0]',zeros(3,n)]; */
    y[i0] = iv1[i0];

    /* 'ForwardRecursion:13' z=[[0 0 1]',zeros(3,n)]; */
    z[i0] = iv2[i0];

    /* 'ForwardRecursion:14' pasterisk=zeros(3,n); */
    /* 'ForwardRecursion:15' jp=zeros(3,n+1); */
    jp[i0] = 0.0;
  }

  /* 'ForwardRecursion:16' for i=1:n */
  for (i = 0; i < 4; i++) {
    /* 'ForwardRecursion:18' x(1:3,i+1)=x(1:3,i)*cos(theta(i))+y(1:3,i)*sin(theta(i)); */
    b_x = cos(thetaold[i]);
    c_x = sin(thetaold[i]);
    for (i0 = 0; i0 < 3; i0++) {
      x[i0 + 3 * (i + 1)] = x[i0 + 3 * i] * b_x + y[i0 + 3 * i] * c_x;
    }

    /* 'ForwardRecursion:19' z(1:3,i+1)=z(1:3,i)*cos(alpha(i))+cross(x(1:3,i+1),z(1:3,i))*sin(alpha(i)); */
    b_x = cos(dv0[i]);
    c_x = sin(dv0[i]);
    ph[0] = x[1 + 3 * (i + 1)] * z[2 + 3 * i] - x[2 + 3 * (i + 1)] * z[1 + 3 * i];
    ph[1] = x[2 + 3 * (i + 1)] * z[3 * i] - x[3 * (i + 1)] * z[2 + 3 * i];
    ph[2] = x[3 * (i + 1)] * z[1 + 3 * i] - x[1 + 3 * (i + 1)] * z[3 * i];
    for (i0 = 0; i0 < 3; i0++) {
      z[i0 + 3 * (i + 1)] = z[i0 + 3 * i] * b_x + ph[i0] * c_x;
    }

    /* 'ForwardRecursion:20' y(1:3,i+1)=cross(z(1:3,i+1),x(1:3,i+1)); */
    y[3 * (1 + i)] = z[1 + 3 * (i + 1)] * x[2 + 3 * (i + 1)] - z[2 + 3 * (i + 1)]
      * x[1 + 3 * (i + 1)];
    y[1 + 3 * (1 + i)] = z[2 + 3 * (i + 1)] * x[3 * (i + 1)] - z[3 * (i + 1)] *
      x[2 + 3 * (i + 1)];
    y[2 + 3 * (1 + i)] = z[3 * (i + 1)] * x[1 + 3 * (i + 1)] - z[1 + 3 * (i + 1)]
      * x[3 * (i + 1)];

    /* 'ForwardRecursion:22' pasterisk(1:3,i)=d(i)*z(1:3,i)+a(i)*x(1:3,i+1); */
    /* 'ForwardRecursion:24' jp(1:3,i+1)=jp(1:3,i)-pasterisk(1:3,i); */
    for (i0 = 0; i0 < 3; i0++) {
      pasterisk[i0 + 3 * i] = 0.0 * z[i0 + 3 * i] + dv1[i] * x[i0 + 3 * (i + 1)];
      jp[i0 + 3 * (i + 1)] = jp[i0 + 3 * i] - pasterisk[i0 + 3 * i];
    }
  }

  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* 'Aktuelle_Position:21' p=-jp; */
  /* 'Aktuelle_Position:22' ph(1:3,1)=p(1:3,5); */
  /* actual position */
  /*  calculate actual rotation with rotation matrices  */
  /* 'Aktuelle_Position:25' thetaold(2)=thetaold(2)-pi/2; */
  thetaold[1] -= 1.5707963267948966;
  for (i0 = 0; i0 < 3; i0++) {
    b_ph[i0] = -jp[12 + i0];

    /* 'Aktuelle_Position:26' thetaold(2:4)=-thetaold(2:4); */
    thetaold[1 + i0] = -thetaold[1 + i0];
  }

  /* 'Aktuelle_Position:27' RotZ1=[cos(thetaold(1)) -sin(thetaold(1)) 0;sin(thetaold(1)) cos(thetaold(1)) 0;0 0 1]; */
  /* 'Aktuelle_Position:28' RotY2=[cos(thetaold(2)) 0 sin(thetaold(2));0 1 0;-sin(thetaold(2)) 0 cos(thetaold(2))]; */
  /* 'Aktuelle_Position:29' RotY3=[cos(thetaold(3)) 0 sin(thetaold(3));0 1 0;-sin(thetaold(3)) 0 cos(thetaold(3))]; */
  /* 'Aktuelle_Position:30' RotY4=[cos(thetaold(4)) 0 sin(thetaold(4));0 1 0;-sin(thetaold(4)) 0 cos(thetaold(4))]; */
  /* 'Aktuelle_Position:31' RotY6=[cos(thetaold(6)) 0 sin(thetaold(6));0 1 0;-sin(thetaold(6)) 0 cos(thetaold(6))]; */
  /* 'Aktuelle_Position:32' RotZ5=[cos(thetaold(5)) -sin(thetaold(5)) 0;sin(thetaold(5)) cos(thetaold(5)) 0;0 0 1]; */
  /* 'Aktuelle_Position:33' RotZ7=[cos(thetaold(7)) -sin(thetaold(7)) 0;sin(thetaold(7)) cos(thetaold(7)) 0;0 0 1]; */
  /* 'Aktuelle_Position:34' R07=RotZ1*RotY2*RotY3*RotY4*RotZ5*RotY6*RotZ7; */
  dv2[0] = cos(thetaold[0]);
  dv2[3] = -sin(thetaold[0]);
  dv2[6] = 0.0;
  dv2[1] = sin(thetaold[0]);
  dv2[4] = cos(thetaold[0]);
  dv2[7] = 0.0;
  dv3[0] = cos(thetaold[1]);
  dv3[3] = 0.0;
  dv3[6] = sin(thetaold[1]);
  for (i0 = 0; i0 < 3; i0++) {
    dv2[2 + 3 * i0] = iv3[i0];
    dv3[1 + 3 * i0] = iv4[i0];
  }

  dv3[2] = -sin(thetaold[1]);
  dv3[5] = 0.0;
  dv3[8] = cos(thetaold[1]);
  dv4[0] = cos(thetaold[2]);
  dv4[3] = 0.0;
  dv4[6] = sin(thetaold[2]);
  for (i0 = 0; i0 < 3; i0++) {
    for (i = 0; i < 3; i++) {
      R07[i0 + 3 * i] = 0.0;
      for (i1 = 0; i1 < 3; i1++) {
        R07[i0 + 3 * i] += dv2[i0 + 3 * i1] * dv3[i1 + 3 * i];
      }
    }

    dv4[1 + 3 * i0] = iv4[i0];
  }

  dv4[2] = -sin(thetaold[2]);
  dv4[5] = 0.0;
  dv4[8] = cos(thetaold[2]);
  dv3[0] = cos(thetaold[3]);
  dv3[3] = 0.0;
  dv3[6] = sin(thetaold[3]);
  for (i0 = 0; i0 < 3; i0++) {
    for (i = 0; i < 3; i++) {
      dv2[i0 + 3 * i] = 0.0;
      for (i1 = 0; i1 < 3; i1++) {
        dv2[i0 + 3 * i] += R07[i0 + 3 * i1] * dv4[i1 + 3 * i];
      }
    }

    dv3[1 + 3 * i0] = iv4[i0];
  }

  dv3[2] = -sin(thetaold[3]);
  dv3[5] = 0.0;
  dv3[8] = cos(thetaold[3]);
  dv4[0] = cos(thetaold[4]);
  dv4[3] = -sin(thetaold[4]);
  dv4[6] = 0.0;
  dv4[1] = sin(thetaold[4]);
  dv4[4] = cos(thetaold[4]);
  dv4[7] = 0.0;
  for (i0 = 0; i0 < 3; i0++) {
    for (i = 0; i < 3; i++) {
      R07[i0 + 3 * i] = 0.0;
      for (i1 = 0; i1 < 3; i1++) {
        R07[i0 + 3 * i] += dv2[i0 + 3 * i1] * dv3[i1 + 3 * i];
      }
    }

    dv4[2 + 3 * i0] = iv3[i0];
  }

  dv3[0] = cos(thetaold[5]);
  dv3[3] = 0.0;
  dv3[6] = sin(thetaold[5]);
  for (i0 = 0; i0 < 3; i0++) {
    for (i = 0; i < 3; i++) {
      dv2[i0 + 3 * i] = 0.0;
      for (i1 = 0; i1 < 3; i1++) {
        dv2[i0 + 3 * i] += R07[i0 + 3 * i1] * dv4[i1 + 3 * i];
      }
    }

    dv3[1 + 3 * i0] = iv4[i0];
  }

  dv3[2] = -sin(thetaold[5]);
  dv3[5] = 0.0;
  dv3[8] = cos(thetaold[5]);
  dv4[0] = cos(thetaold[6]);
  dv4[3] = -sin(thetaold[6]);
  dv4[6] = 0.0;
  dv4[1] = sin(thetaold[6]);
  dv4[4] = cos(thetaold[6]);
  dv4[7] = 0.0;
  for (i0 = 0; i0 < 3; i0++) {
    for (i = 0; i < 3; i++) {
      R07[i0 + 3 * i] = 0.0;
      for (i1 = 0; i1 < 3; i1++) {
        R07[i0 + 3 * i] += dv2[i0 + 3 * i1] * dv3[i1 + 3 * i];
      }
    }

    dv4[2 + 3 * i0] = iv3[i0];
  }

  for (i0 = 0; i0 < 3; i0++) {
    for (i = 0; i < 3; i++) {
      b_R07[i0 + 3 * i] = 0.0;
      for (i1 = 0; i1 < 3; i1++) {
        b_R07[i0 + 3 * i] += R07[i0 + 3 * i1] * dv4[i1 + 3 * i];
      }
    }
  }

  /*  calcualte endeffektor position */
  /* 'Aktuelle_Position:37' ph=ph+R07*l_endeffektor; */
  for (i0 = 0; i0 < 3; i0++) {
    b_x = 0.0;
    for (i = 0; i < 3; i++) {
      b_x += b_R07[i0 + 3 * i] * b[i];
    }

    b_ph[i0] += b_x;
  }

  /* 'Aktuelle_Position:42' RZ=[cos(rd(1)) -sin(rd(1)) 0;sin(rd(1)) cos(rd(1)) 0;0 0 1]; */
  /* 'Aktuelle_Position:43' RX=[1 0 0;0 cos(rd(2)) -sin(rd(2));0 sin(rd(2)) cos(rd(2))]; */
  /* 'Aktuelle_Position:44' RY=[cos(rd(3)) 0 sin(rd(3));0 1 0;-sin(rd(3)) 0 cos(rd(3))]; */
  /* calcualte desired endeffector rotation */
  /* 'Aktuelle_Position:47' R_des=R07*RZ*RY*RX; */
  dv2[0] = cos(rd[0]);
  dv2[3] = -sin(rd[0]);
  dv2[6] = 0.0;
  dv2[1] = sin(rd[0]);
  dv2[4] = cos(rd[0]);
  dv2[7] = 0.0;
  for (i0 = 0; i0 < 3; i0++) {
    dv2[2 + 3 * i0] = iv3[i0];
  }

  dv3[0] = cos(rd[2]);
  dv3[3] = 0.0;
  dv3[6] = sin(rd[2]);
  for (i0 = 0; i0 < 3; i0++) {
    for (i = 0; i < 3; i++) {
      R07[i0 + 3 * i] = 0.0;
      for (i1 = 0; i1 < 3; i1++) {
        R07[i0 + 3 * i] += b_R07[i0 + 3 * i1] * dv2[i1 + 3 * i];
      }
    }

    dv3[1 + 3 * i0] = iv4[i0];
  }

  dv3[2] = -sin(rd[2]);
  dv3[5] = 0.0;
  dv3[8] = cos(rd[2]);
  for (i0 = 0; i0 < 3; i0++) {
    for (i = 0; i < 3; i++) {
      b_R07[i0 + 3 * i] = 0.0;
      for (i1 = 0; i1 < 3; i1++) {
        b_R07[i0 + 3 * i] += R07[i0 + 3 * i1] * dv3[i1 + 3 * i];
      }
    }

    dv2[3 * i0] = iv5[i0];
  }

  dv2[1] = 0.0;
  dv2[4] = cos(rd[1]);
  dv2[7] = -sin(rd[1]);
  dv2[2] = 0.0;
  dv2[5] = sin(rd[1]);
  dv2[8] = cos(rd[1]);
  for (i0 = 0; i0 < 3; i0++) {
    for (i = 0; i < 3; i++) {
      rd_3x3[i0 + 3 * i] = 0.0;
      for (i1 = 0; i1 < 3; i1++) {
        rd_3x3[i0 + 3 * i] += b_R07[i0 + 3 * i1] * dv2[i1 + 3 * i];
      }
    }
  }

  /*  consider coordinate frame and calcualte new position for CCD */
  /* 'Aktuelle_Position:50' if kord == 0 */
  if (kord == 0.0) {
    /* 'Aktuelle_Position:51' pd=ph+pd -R_des*l_endeffektor; */
    for (i0 = 0; i0 < 3; i0++) {
      b_x = 0.0;
      for (i = 0; i < 3; i++) {
        b_x += rd_3x3[i0 + 3 * i] * b[i];
      }

      pd[i0] = (b_ph[i0] + pd[i0]) - b_x;
    }

    /* global coordiante frame */
    /* 'Aktuelle_Position:53' max_pos = sqrt(pd(1)*pd(1)+pd(2)*pd(2)+pd(3)*pd(3)) */
    /* 'Aktuelle_Position:55' if (max_pos > 1.22) */
    if (sqrt((pd[0] * pd[0] + pd[1] * pd[1]) + pd[2] * pd[2]) > spaceConstraint) {
      /* 'Aktuelle_Position:56' pd=ph-pd_input -R_des*l_endeffektor  */
      for (i0 = 0; i0 < 3; i0++) {
        b_x = 0.0;
        for (i = 0; i < 3; i++) {
          b_x += rd_3x3[i0 + 3 * i] * b[i];
        }

        pd[i0] = (b_ph[i0] - pd_input[i0]) - b_x;
      }
    }
  } else {
    if (kord == 1.0) {
      /* 'Aktuelle_Position:60' elseif kord == 1 */
      /* 'Aktuelle_Position:61' pd=ph+R_des*pd -R_des*l_endeffektor; */
      for (i0 = 0; i0 < 3; i0++) {
        b_x = 0.0;
        for (i = 0; i < 3; i++) {
          b_x += rd_3x3[i0 + 3 * i] * pd[i];
        }

        ph[i0] = b_ph[i0] + b_x;
      }

      for (i0 = 0; i0 < 3; i0++) {
        b_rd_3x3[i0] = 0.0;
        for (i = 0; i < 3; i++) {
          b_rd_3x3[i0] += rd_3x3[i0 + 3 * i] * b[i];
        }
      }


      ROS_INFO_STREAM("You are using Endeffector-Coordinate-System");
      for (i0 = 0; i0 < 3; i0++) {
        pd[i0] = ph[i0] - b_rd_3x3[i0];
      }

      /* end-effector coordiante frame */
      /* 'Aktuelle_Position:63' max_pos = sqrt(pd(1)*pd(1)+pd(2)*pd(2)+pd(3)*pd(3)) */
      /* 'Aktuelle_Position:65' if (max_pos > 1.22) */
      if (sqrt((pd[0] * pd[0] + pd[1] * pd[1]) + pd[2] * pd[2]) > spaceConstraint) {
        /* 'Aktuelle_Position:66' pd=ph-R_des*pd_input -R_des*l_endeffektor  */
        for (i0 = 0; i0 < 3; i0++) {
          b_x = 0.0;
          for (i = 0; i < 3; i++) {
            b_x += rd_3x3[i0 + 3 * i] * pd_input[i];
          }

          ph[i0] = b_ph[i0] - b_x;
        }

        for (i0 = 0; i0 < 3; i0++) {
          b_rd_3x3[i0] = 0.0;
          for (i = 0; i < 3; i++) {
            b_rd_3x3[i0] += rd_3x3[i0 + 3 * i] * b[i];
          }
        }

        for (i0 = 0; i0 < 3; i0++) {
          pd[i0] = ph[i0] - b_rd_3x3[i0];
        }
      }
    }
  }

  /* 'Aktuelle_Position:71' rd_3x3 = R_des; */
  /*  new rotation matrix for CCD */
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void Aktuelle_Position_initialize(void)
{
  rt_InitInfAndNaN(8U);
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void Aktuelle_Position_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for Aktuelle_Position.c
 *
 * [EOF]
 */

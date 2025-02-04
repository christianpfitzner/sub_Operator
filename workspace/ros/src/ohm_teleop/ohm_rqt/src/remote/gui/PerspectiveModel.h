/*
 * PerspectiveModel.h
 *
 *  Created on: Jun 6, 2018
 *      Author: phil
 */

#ifndef OHM_SCHR_REMOTE_SRC_HUD_PERSPECTIVEMODEL_H_
#define OHM_SCHR_REMOTE_SRC_HUD_PERSPECTIVEMODEL_H_

#include <vector>
#include <QtGui/QPainter>
#include <Eigen/Dense>


typedef std::vector<Eigen::Vector4d, Eigen::aligned_allocator<Eigen::Vector4d> > stdVecEig4d;

namespace
{
//  const Eigen::Translation<double, 3> t_FFL( 1.4,  1.5, 0.25);
//  const Eigen::Translation<double, 3> t_FFR( 1.4, -1.8, 0.25);
//  const Eigen::Translation<double, 3> t_FRL(-1.4,  1.5, 0.25);
//  const Eigen::Translation<double, 3> t_FRR(-1.4, -1.8, 0.25);
}

namespace ohm_remote
{


class PerspectiveModel
{
public:
  enum class Points
   {
     CHASS_LOW_FRONT_LEFT = 0,
     CHASS_LOW_FRONT_RIGHT,
     CHASS_LOW_REAR_LEFT,
     CHASS_LOW_REAR_RIGHT,
     CHASS_TOP_FRONT_LEFT,
     CHASS_TOP_FRONT_RIGHT,
     CHASS_TOP_REAR_LEFT,
     CHASS_TOP_REAR_RIGHT,
     FLIPP_B0_L,
     FLIPP_B1_L,
     FLIPP_B0_R,
     FLIPP_B1_R,
     FLIPP_T_L,
     FLIPP_T_R
   };
   enum class Flipper
   {
     FRONT_LEFT = 0,
     FRONT_RIGHT,
     REAR_LEFT,
     REAR_RIGHT
   };
   enum class SubPointsFlipper
   {
     L_BASE_0 = 0,
     L_BASE_1,
     L_TOP,
     R_BASE_0,
     R_BASE_1,
     R_TOP
   };
   enum class SubPointsChassis
   {
     LW_FR_L = 0,
     LW_FR_R,
     LW_RE_L,
     LW_RE_R,
     UP_FR_L,
     UP_FR_R,
     UP_RE_L,
     UP_RE_R
   };
   enum class BodyTrns
   {
     T_FFL = 0,
     T_FFR,
     T_FRL,
     T_FRR
   };
  PerspectiveModel(const double width, const double widthY, const double height, const double flipWidth, const double flipHeight, double flipEdget);
  virtual ~PerspectiveModel();
  void paintPerspectiveModel(const std::vector<double>& anglesFlipper, QPainter* const painter);
  void setAngleFlippers(std::vector<double>* const angles){_angleFlippers = *angles;} //todo: add failsave method
  void perspectiveTransform(void);
  void resize(const double width, const double widthY, const double height, const double flipWidth, const double flipHeight, double flipEdget);
private:

  QPoint backProject(const Eigen::Vector4d& point);
  bool transfomFlippers(QVector<QVector<QPoint> >& points);
  bool transformFlipper(const Flipper& flipper, stdVecEig4d* const points, const double angle, const Eigen::Matrix4d& Tinv);
  bool transformChassis(stdVecEig4d* const points, const Eigen::Matrix4d& Tinv);
  void generateModel(void);
  Eigen::Vector4d transformEigen3(const Eigen::Vector4d& input, const Eigen::Translation<double, 3>& t, const Eigen::AngleAxis<double>& R);
  void paintChassis(QPainter* const painter);
  void paintFlipper(QPainter* const painter);
  const Eigen::AngleAxis<double> _rInity;
  const Eigen::AngleAxis<double> _rInitx;
  Eigen::Matrix<double, 3, 4> _P;
    Eigen::Vector3d _t;
    double _pitch;
    double _flipFLRot;
    stdVecEig4d _model;
    QVector<QPoint> _pixelChassis;
    QVector<QVector<QPoint> > _pixelFlippers;
    std::vector<double> _angleFlippers;
    double _width     ;
    double _widthY;
    double _height    ;
    double _flipWidth ;
    double _flipHeight;
    double _flipEdget ;
    std::vector<Eigen::Translation<double, 3> > _bodyTrns;
};

}

#endif /* OHM_SCHR_REMOTE_SRC_HUD_PERSPECTIVEMODEL_H_ */

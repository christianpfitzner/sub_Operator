/*
 * PerspectiveModel.cpp
 *
 *  Created on: Jun 6, 2018
 *      Author: phil
 */

#include "PerspectiveModel.h"
#include <iostream>
#include <QtCore/QDebug>

namespace ohm_remote
{

PerspectiveModel::PerspectiveModel(const double width, const double widthY, const double height, const double flipWidth, const double flipHeight, double flipEdget):
_rInity(Eigen::AngleAxis<double>(M_PI / 2.0, Eigen::Vector3d::UnitY())),
_rInitx(Eigen::AngleAxis<double>(-M_PI / 2.0, Eigen::Vector3d::UnitX())),
_pitch(0.0),
_flipFLRot(0.0),
_model(32, Eigen::Vector4d(0.0, 0.0, 0.0, 1.0)),
_pixelChassis(8, QPoint(0, 0)),
_pixelFlippers(4),
_angleFlippers(4, 1.0),
_width     (width),
_widthY    (widthY),
_height    (height),
_flipWidth (flipWidth),
_flipHeight(flipHeight),
_flipEdget (flipEdget)
{
  double Pdata[12] = {561.065430/2, 0.0, 318.726308/2.0, 0.0,
       0.0, 562.793762/2.0, 249.564153/2, 0.0,
       0.0, 0.0, 1.0, 0.000000};
   _P = (Eigen::Map<Eigen::Matrix<double, 3, 4, Eigen::RowMajor> >(Pdata));
   _pitch = static_cast<double>(94) / 100.0;   //todo: some magic numbers here. Maybe allow altering it from a submenu
   _t(0) = -static_cast<double>(4.0);
   _t(1) = 0.0;
   _t(2) = static_cast<double>(3.0);

   _bodyTrns.push_back(Eigen::Translation<double, 3>((width / 2.0 - flipWidth / 2.0),  (widthY / 2.0), height / 2.0));
   _bodyTrns.push_back(Eigen::Translation<double, 3>((width / 2.0 - flipWidth / 2.0), -(widthY / 2.0 + flipWidth), height / 2.0));
   _bodyTrns.push_back(Eigen::Translation<double, 3>(-(width / 2.0 - flipWidth / 2.0), (widthY / 2.0), height / 2.0));
   _bodyTrns.push_back(Eigen::Translation<double, 3>(-(width / 2.0 - flipWidth / 2.0), -(widthY / 2.0 + flipWidth), height / 2.0));



   this->generateModel();
   this->perspectiveTransform();
}

PerspectiveModel::~PerspectiveModel()
{
  // TODO Auto-generated destructor stub
}

void PerspectiveModel::perspectiveTransform(void)
{
//_guiui->widget->updatePoints(points);
//  QVector<QPoint> pixelChassis(8, QPoint(0, 0));
//  QVector<QVector<QPoint> > pixelFlippers(4);

  Eigen::Matrix4d T;
  Eigen::Matrix3d R;
 // const Eigen::AngleAxis<double> initRot = Eigen::AngleAxis<double>(M_PI / 2.0, Eigen::Vector3d::UnitX());
  const Eigen::AngleAxis<double> pitch = Eigen::AngleAxis<double>(_pitch, Eigen::Vector3d::UnitY());
  T.setIdentity();
  R.setIdentity();
  R = (_rInitx *  _rInity).matrix();
  //  R = _rInitx.matrix() * _rInitx.matrix() * R;
  R = pitch.matrix() * R;

  T.block(0, 0, 3, 3) = R;
  T(0, 3) = _t(0);
  T(1, 3) = _t(1);
  T(2, 3) = _t(2);

// std::cout << __PRETTY_FUNCTION__  << " T : \n" << T << std::endl;

  Eigen::Matrix4d Tinv = T.inverse();
  stdVecEig4d pointsChassis(8, Eigen::Vector4d(0.0, 0.0, 0.0, 1.0));
  this->transformChassis(&pointsChassis, Tinv);

//  std::cout << __PRETTY_FUNCTION__ << " backproj chassis " << std::endl;
  for(unsigned int i = static_cast<unsigned int>(SubPointsChassis::LW_FR_L); i <= static_cast<unsigned int>(SubPointsChassis::UP_RE_R); i++)
  {
  //  std::cout << __PRETTY_FUNCTION__ << "  " << pointsChassis[i] << std::endl;
    _pixelChassis[i] = this->backProject(pointsChassis[i]);
  }
  //std::cout << __PRETTY_FUNCTION__ << " backproj flippers " << std::endl;
  for(unsigned int i = static_cast<unsigned int>(Flipper::FRONT_LEFT); i  <= static_cast<unsigned int>(Flipper::REAR_RIGHT); i++)
  {
    stdVecEig4d pointsFlipper(6, Eigen::Vector4d(0.0, 0.0, 0.0, 1.0));
    this->transformFlipper(static_cast<Flipper>(i), &pointsFlipper, _angleFlippers[i], Tinv);
    QVector<QPoint> pxlsFlip(6, QPoint(0, 0));
    for(unsigned int j = static_cast<unsigned int>(SubPointsFlipper::L_BASE_0); j <= static_cast<unsigned int>(SubPointsFlipper::R_TOP); j++)
    {
      pxlsFlip[j] = this->backProject(pointsFlipper[j]);
    }
    _pixelFlippers[i] = pxlsFlip;
  }
}

QPoint PerspectiveModel::backProject(const Eigen::Vector4d& point)
{
  Eigen::Vector3d pxlVec;
  pxlVec = _P * point;
  QPoint qPoint = QPoint(static_cast<int>(round(pxlVec(0) / pxlVec(2))), static_cast<int>(round(pxlVec(1) / pxlVec(2))));
  return qPoint;
}

bool PerspectiveModel::transformFlipper(const Flipper& flipper, stdVecEig4d* const points, const double angle, const Eigen::Matrix4d& Tinv)
{
  if(points->size() != 6)
  {
    std::cout << __PRETTY_FUNCTION__ << " error! Input vector has the wrong size " << points->size() << " should be 6" << std::endl;
    return false;
  }
  Eigen::Translation<double, 3> t;
  (*points)[static_cast<unsigned int>(SubPointsFlipper::L_BASE_0)] = _model[static_cast<unsigned int>(Points::FLIPP_B0_L)];
  (*points)[static_cast<unsigned int>(SubPointsFlipper::L_BASE_1)] = _model[static_cast<unsigned int>(Points::FLIPP_B1_L)];
  (*points)[static_cast<unsigned int>(SubPointsFlipper::R_BASE_0)] = _model[static_cast<unsigned int>(Points::FLIPP_B0_R)];
  (*points)[static_cast<unsigned int>(SubPointsFlipper::R_BASE_1)] = _model[static_cast<unsigned int>(Points::FLIPP_B1_R)];
  (*points)[static_cast<unsigned int>(SubPointsFlipper::L_TOP   )] = _model[static_cast<unsigned int>(Points::FLIPP_T_L)];
  (*points)[static_cast<unsigned int>(SubPointsFlipper::R_TOP   )] = _model[static_cast<unsigned int>(Points::FLIPP_T_R)];
  switch(flipper)
  {
  case Flipper::FRONT_LEFT:
  {
    //t = ::t_FFL;
    t = _bodyTrns[static_cast<unsigned int>(BodyTrns::T_FFL)];
    break;
  }
  case Flipper::FRONT_RIGHT:
  {
    //t = ::t_FFR;
    t = _bodyTrns[static_cast<unsigned int>(BodyTrns::T_FFR)];
    break;
  }
  case Flipper::REAR_LEFT:
  {
   // t = ::t_FRL;
    t = _bodyTrns[static_cast<unsigned int>(BodyTrns::T_FRL)];
    break;
  }
  case Flipper::REAR_RIGHT:
  {
    //t = ::t_FRR;
    t = _bodyTrns[static_cast<unsigned int>(BodyTrns::T_FRR)];
    break;
  }
  default:
  {
    std::cout << __PRETTY_FUNCTION__ << " error! Unknown flipper selected " << std::endl;
    return false;
    break;
  }
  }
  const Eigen::AngleAxis<double> flipRot = Eigen::AngleAxis<double>(angle, Eigen::Vector3d::UnitY());
  (*points)[static_cast<unsigned int>(SubPointsFlipper::L_BASE_0)] = Tinv * this->transformEigen3((*points)[static_cast<unsigned int>(SubPointsFlipper::L_BASE_0)], t, flipRot);
  (*points)[static_cast<unsigned int>(SubPointsFlipper::L_BASE_1)] = Tinv * this->transformEigen3((*points)[static_cast<unsigned int>(SubPointsFlipper::L_BASE_1)], t, flipRot);
  (*points)[static_cast<unsigned int>(SubPointsFlipper::R_BASE_0)] = Tinv * this->transformEigen3((*points)[static_cast<unsigned int>(SubPointsFlipper::R_BASE_0)], t, flipRot);
  (*points)[static_cast<unsigned int>(SubPointsFlipper::R_BASE_1)] = Tinv * this->transformEigen3((*points)[static_cast<unsigned int>(SubPointsFlipper::R_BASE_1)], t, flipRot);
  (*points)[static_cast<unsigned int>(SubPointsFlipper::L_TOP)]    = Tinv * this->transformEigen3((*points)[static_cast<unsigned int>(SubPointsFlipper::L_TOP)], t, flipRot);
  (*points)[static_cast<unsigned int>(SubPointsFlipper::R_TOP)]    = Tinv * this->transformEigen3((*points)[static_cast<unsigned int>(SubPointsFlipper::R_TOP)], t, flipRot);
  return true;
}

bool PerspectiveModel::transformChassis(stdVecEig4d* const points, const Eigen::Matrix4d& Tinv)
{
  if(points->size() != 8)
  {
    std::cout << __PRETTY_FUNCTION__ << " wrong vector size " << points->size() << " instead of 8 " << std::endl;
    return false;
  }
  (*points)[static_cast<unsigned int>(SubPointsChassis::LW_FR_L)] = Tinv * _model[static_cast<unsigned int>(Points::CHASS_LOW_FRONT_LEFT )];
  (*points)[static_cast<unsigned int>(SubPointsChassis::LW_FR_R)] = Tinv * _model[static_cast<unsigned int>(Points::CHASS_LOW_FRONT_RIGHT)];
  (*points)[static_cast<unsigned int>(SubPointsChassis::LW_RE_L)] = Tinv * _model[static_cast<unsigned int>(Points::CHASS_LOW_REAR_LEFT  )];
  (*points)[static_cast<unsigned int>(SubPointsChassis::LW_RE_R)] = Tinv * _model[static_cast<unsigned int>(Points::CHASS_LOW_REAR_RIGHT )];
  (*points)[static_cast<unsigned int>(SubPointsChassis::UP_FR_L)] = Tinv * _model[static_cast<unsigned int>(Points::CHASS_TOP_FRONT_LEFT )];
  (*points)[static_cast<unsigned int>(SubPointsChassis::UP_FR_R)] = Tinv * _model[static_cast<unsigned int>(Points::CHASS_TOP_FRONT_RIGHT)];
  (*points)[static_cast<unsigned int>(SubPointsChassis::UP_RE_L)] = Tinv * _model[static_cast<unsigned int>(Points::CHASS_TOP_REAR_LEFT  )];
  (*points)[static_cast<unsigned int>(SubPointsChassis::UP_RE_R)] = Tinv * _model[static_cast<unsigned int>(Points::CHASS_TOP_REAR_RIGHT )];
  return false;
}

void PerspectiveModel::generateModel(void)
{
  //const double width      = 3.0;
  //const double height     = 0.5;
  //const double flipWidth  = 0.2;
  //const double flipHeight = 0.4;
  //const double flipEdget  = 1.0;

  _model[static_cast<unsigned int>(Points::CHASS_LOW_FRONT_LEFT )] = Eigen::Vector4d( _width / 2.0,  _widthY / 2.0, 0.0, 1.0);
  _model[static_cast<unsigned int>(Points::CHASS_LOW_FRONT_RIGHT)] = Eigen::Vector4d( _width / 2.0, -_widthY / 2.0, 0.0, 1.0);
  _model[static_cast<unsigned int>(Points::CHASS_LOW_REAR_LEFT  )] = Eigen::Vector4d(-_width / 2.0,  _widthY / 2.0, 0.0, 1.0);
  _model[static_cast<unsigned int>(Points::CHASS_LOW_REAR_RIGHT )] = Eigen::Vector4d(-_width / 2.0, -_widthY / 2.0, 0.0, 1.0);
  _model[static_cast<unsigned int>(Points::CHASS_TOP_FRONT_LEFT )] = Eigen::Vector4d( _width / 2.0,  _widthY / 2.0, _height, 1.0);
  _model[static_cast<unsigned int>(Points::CHASS_TOP_FRONT_RIGHT)] = Eigen::Vector4d( _width / 2.0, -_widthY / 2.0, _height, 1.0);
  _model[static_cast<unsigned int>(Points::CHASS_TOP_REAR_LEFT  )] = Eigen::Vector4d(-_width / 2.0,  _widthY / 2.0, _height, 1.0);
  _model[static_cast<unsigned int>(Points::CHASS_TOP_REAR_RIGHT )] = Eigen::Vector4d(-_width / 2.0, -_widthY / 2.0, _height, 1.0);
  _model[static_cast<unsigned int>(Points::FLIPP_B0_L)]            = Eigen::Vector4d( _flipWidth / 2.0,  0.0, 0.0, 1.0);
  _model[static_cast<unsigned int>(Points::FLIPP_B1_L)]            = Eigen::Vector4d(-_flipWidth / 2.0,  0.0, 0.0, 1.0);
  _model[static_cast<unsigned int>(Points::FLIPP_B0_R)]            = Eigen::Vector4d( _flipWidth / 2.0,  _flipHeight, 0.0, 1.0);
  _model[static_cast<unsigned int>(Points::FLIPP_B1_R)]            = Eigen::Vector4d(-_flipWidth / 2.0,  _flipHeight, 0.0, 1.0);
  _model[static_cast<unsigned int>(Points::FLIPP_T_L)]             = Eigen::Vector4d( 0.0,  0.0, _flipEdget, 1.0);
  _model[static_cast<unsigned int>(Points::FLIPP_T_R)]             = Eigen::Vector4d( 0.0,  _flipHeight, _flipEdget, 1.0);
}

Eigen::Vector4d PerspectiveModel::transformEigen3(const Eigen::Vector4d& input, const Eigen::Translation<double, 3>& t, const Eigen::AngleAxis<double>& R)
{
  Eigen::Vector3d var(input.x(), input.y(), input.z());
  var = t * R * var;
  return Eigen::Vector4d(var.x(), var.y(), var.z(), 1.0);
}

void PerspectiveModel::paintChassis(QPainter* const painter)
{
//  std::cout << __PRETTY_FUNCTION__ << " paint chassis " << std::endl;
  painter->drawLine(_pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::LW_FR_L)],
                    _pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::LW_FR_R)]);
//  qDebug() << __PRETTY_FUNCTION__ << " " << _pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::LW_FR_R)]
//                                         << _pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::LW_FR_L)];
  painter->drawLine(_pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::LW_RE_L)],
                    _pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::LW_RE_R)]);
  painter->drawLine(_pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::LW_FR_L)],
                    _pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::LW_RE_L)]);
  painter->drawLine(_pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::LW_FR_R)],
                    _pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::LW_RE_R)]);
  painter->drawLine(_pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::UP_FR_L)],
                    _pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::UP_FR_R)]);
  painter->drawLine(_pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::UP_RE_L)],
                    _pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::UP_RE_R)]);
  painter->drawLine(_pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::UP_FR_L)],
                    _pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::UP_RE_L)]);
  painter->drawLine(_pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::UP_FR_R)],
                    _pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::UP_RE_R)]);
  painter->drawLine(_pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::UP_FR_L)],
                    _pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::LW_FR_L)]);
  painter->drawLine(_pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::UP_FR_R)],
                    _pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::LW_FR_R)]);
  painter->drawLine(_pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::UP_RE_L)],
                    _pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::LW_RE_L)]);
  painter->drawLine(_pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::UP_RE_R)],
                    _pixelChassis[static_cast<unsigned int>(PerspectiveModel::SubPointsChassis::LW_RE_R)]);
}

void PerspectiveModel::paintFlipper(QPainter* const painter)
{
  for(unsigned int i = 0; i < _pixelFlippers.size(); i++)
  {
    painter->drawLine(_pixelFlippers[i][static_cast<unsigned int>(PerspectiveModel::SubPointsFlipper::L_BASE_0)],
                      _pixelFlippers[i][static_cast<unsigned int>(PerspectiveModel::SubPointsFlipper::L_BASE_1)]);
    painter->drawLine(_pixelFlippers[i][static_cast<unsigned int>(PerspectiveModel::SubPointsFlipper::L_BASE_0)],
                      _pixelFlippers[i][static_cast<unsigned int>(PerspectiveModel::SubPointsFlipper::L_TOP)]);
    painter->drawLine(_pixelFlippers[i][static_cast<unsigned int>(PerspectiveModel::SubPointsFlipper::L_BASE_1)],
                      _pixelFlippers[i][static_cast<unsigned int>(PerspectiveModel::SubPointsFlipper::L_TOP)]);
    painter->drawLine(_pixelFlippers[i][static_cast<unsigned int>(PerspectiveModel::SubPointsFlipper::R_BASE_0)],
                      _pixelFlippers[i][static_cast<unsigned int>(PerspectiveModel::SubPointsFlipper::R_BASE_1)]);
    painter->drawLine(_pixelFlippers[i][static_cast<unsigned int>(PerspectiveModel::SubPointsFlipper::R_BASE_0)],
                      _pixelFlippers[i][static_cast<unsigned int>(PerspectiveModel::SubPointsFlipper::R_TOP)]);
    painter->drawLine(_pixelFlippers[i][static_cast<unsigned int>(PerspectiveModel::SubPointsFlipper::R_BASE_1)],
                      _pixelFlippers[i][static_cast<unsigned int>(PerspectiveModel::SubPointsFlipper::R_TOP)]);
    painter->drawLine(_pixelFlippers[i][static_cast<unsigned int>(PerspectiveModel::SubPointsFlipper::L_BASE_0)],
                      _pixelFlippers[i][static_cast<unsigned int>(PerspectiveModel::SubPointsFlipper::R_BASE_0)]);
    painter->drawLine(_pixelFlippers[i][static_cast<unsigned int>(PerspectiveModel::SubPointsFlipper::L_BASE_1)],
                      _pixelFlippers[i][static_cast<unsigned int>(PerspectiveModel::SubPointsFlipper::R_BASE_1)]);
    painter->drawLine(_pixelFlippers[i][static_cast<unsigned int>(PerspectiveModel::SubPointsFlipper::L_TOP)],
                      _pixelFlippers[i][static_cast<unsigned int>(PerspectiveModel::SubPointsFlipper::R_TOP)]);
  }
}

void PerspectiveModel::paintPerspectiveModel(const std::vector<double>& anglesFlipper, QPainter* const painter)
{
  this->paintChassis(painter);
  this->paintFlipper(painter);
}

void PerspectiveModel::resize(const double width, const double widthY, const double height, const double flipWidth, const double flipHeight, double flipEdget)
{
  _width = width;
  _widthY = widthY;
  _height = height;
  _flipWidth = flipWidth;
  _flipHeight = flipHeight;
  _flipEdget = flipEdget;

  _bodyTrns.clear();
  _bodyTrns.push_back(Eigen::Translation<double, 3>((width / 2.0 - flipWidth / 2.0),  (widthY / 2.0), height / 2.0));
  _bodyTrns.push_back(Eigen::Translation<double, 3>((width / 2.0 - flipWidth / 2.0), -(widthY / 2.0 + flipWidth), height / 2.0));
  _bodyTrns.push_back(Eigen::Translation<double, 3>(-(width / 2.0 - flipWidth / 2.0), (widthY / 2.0), height / 2.0));
  _bodyTrns.push_back(Eigen::Translation<double, 3>(-(width / 2.0 - flipWidth / 2.0), -(widthY / 2.0 + flipWidth), height / 2.0));

  this->generateModel();
  this->perspectiveTransform();
}

}

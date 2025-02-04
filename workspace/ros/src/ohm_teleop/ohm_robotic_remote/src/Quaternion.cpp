#include "Quaternion.h"

#include <tf/tf.h>

Quaternion::Quaternion(const float x, const float y, const float z, const float w)
    : _x(x),
      _y(y),
      _z(z),
      _w(w)
{
    tf::Quaternion quat(x, y, z, w);
    double roll, pitch, yaw;
    tf::Matrix3x3(quat).getRPY(roll, pitch, yaw);

    _roll = roll;
    _pitch = pitch;
    _yaw = yaw;
}

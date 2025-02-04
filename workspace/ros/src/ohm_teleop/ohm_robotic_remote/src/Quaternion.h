#ifndef __QUATERNION_H__
#define __QUATERNION_H__

class Quaternion
{
public:
    Quaternion(const float x, const float y, const float z, const float w);

    float roll(void) const { return _roll; }
    float pitch(void) const { return _pitch; }
    float yaw(void) const { return _yaw; }

private:
    float _x;
    float _y;
    float _z;
    float _w;
    float _roll;
    float _pitch;
    float _yaw;
};

#endif

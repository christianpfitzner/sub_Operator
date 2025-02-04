/*
 * Vector3.h
 *
 *  Created on: 25.03.2015
 *      Author: feesma44884
 */

#ifndef OHM_INSPECTOR_FIRMWARE_SRC_INSPECTORMATH_H_
#define OHM_INSPECTOR_FIRMWARE_SRC_INSPECTORMATH_H_

#include "definitions.h"
#include "core/inc/wirish.h"

/**
 * Math classes for the inspector
 */
namespace InspectorMath
{

/**
 * Represents a three dimensional vector
 */
class Vector3
{
public:

    /**
     * Default constructor
     */
    Vector3()   :   _x(0.0f), _y(0.0f), _z(0.0f)    {}

    /**
     * Creates vector with parameters x, y and z
     *
     * @param x[in] x value
     * @param y[in] y value
     * @param z[in] z value
     */
    Vector3(const float& x, const float& y, const float& z) : _x(x), _y(y), _z(z)   {}

    /**
     * Creates new vector with the data from v
     *
     * @param v[in] vector
     */
    Vector3(const Vector3& v)   : _x(v._x), _y(v._y), _z(v._z) {}


    float   _x; /**<X - direction*/
    float   _y; /**<Y - direction*/
    float   _z; /**<Z - direction*/
};

/**
 * result = a + b
 *
 * @param a[in]
 * @param b[in]
 *
 * @return result of operation
 */
inline Vector3 operator + (const Vector3& a, const Vector3& b)  {return Vector3(a._x + b._x, a._y + b._y, a._z + b._z);}

/**
 * result = a - b
 *
 * @param a[in]
 * @param b[in]
 *
 * @return result of operation
 */
inline Vector3 operator - (const Vector3& a, const Vector3& b)  {return Vector3(a._x - b._x, a._y - b._y, a._z - b._z);}

/**
 * result = a * f
 *
 * @param a[in]
 * @param f[in]
 *
 * @return result of operation
 */
inline Vector3 operator * (const Vector3& a, const float& f)    {return Vector3(a._x * f, a._y * f, a._z * f);}

/**
 * result = f * a
 *
 * @param f[in]
 * @param a[in]
 *
 * @return result of operation
 */
inline Vector3 operator * (const float f, const Vector3& a)     {return Vector3(a._x * f, a._y * f, a._z * f);}

/**
 * result = a * b
 *
 * @param a[in]
 * @param b[in]
 *
 * @return result of operation
 */
inline Vector3 operator * (const Vector3& a, const Vector3& b)  {return Vector3(a._x * b._x, a._y * b._y, a._z * b._z);}

/**
 * result = a / f
 *
 * @param a[in]
 * @param f[in]
 *
 * @return result of operation
 */
inline Vector3 operator / (const Vector3& a, const float& f)    {return Vector3(a._x / f, a._y / f, a._z / f);}

/**
 * Computes the length of the vector v
 *
 * @param v[in] vector
 * @return length
 */
inline float    Vector3Length(const Vector3& v)                    {return sqrtf(v._x * v._x + v._y * v._y + v._z * v._z);}

/**
 * Normalizes the vector v
 *
 * @param v[in] vector
 * @return normalized vector
 */
inline Vector3  Vector3Normalize(const Vector3& v)                 {return Vector3(v / sqrtf(v._x * v._x + v._y * v._y + v._z * v._z));}

/**
 * Computes the dot product of vector a and b
 *
 * @param a[in] vector a
 * @param b[in] vector b
 * @return dot product of vector a and b
 */
inline float    Vector3Dot(const Vector3& a, const Vector3& b)     {return (a._x * b._x + a._y * b._y + a._z * b._z);}

/**
 * Represents a 2D plane
 */
class Plane
{
public:

    /**
     * Default constructor
     */
    Plane() : _vNormal(0.0f, 1.0f, 0.0f), _vPosition()   {}

    /**
     * Creates a plane from normal vector and position
     *
     * @param normal[in] Normal vector describing the orientation of the plane
     * @param position[in] Position describing the position of the normal vector in 3D space
     */
    Plane(const Vector3& normal, const Vector3 &position);

    /**
     * Computes the distance from the plane to the point
     *
     * @param point[in]: Position of the point in 3D space
     *
     * @return distance from point to plane
     */
    float distance(const Vector3& point);

private:

    Vector3     _vNormal;   /**<Normal vector describing orientation of the plane*/
    Vector3     _vPosition; /**<Position of the normal vector*/

};
}

#endif /* OHM_INSPECTOR_FIRMWARE_SRC_INSPECTORMATH_H_ */

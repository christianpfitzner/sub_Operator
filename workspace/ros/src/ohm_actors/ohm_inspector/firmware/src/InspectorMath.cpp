/*
 * InspectorMath.cpp
 *
 *  Created on: 25.03.2015
 *      Author: feesma44884
 */
#include "InspectorMath.h"

namespace InspectorMath
{

Plane::Plane(const Vector3& normal, const Vector3& position) :
_vNormal(),
_vPosition(position)
{
    //normalize vector
    _vNormal    =   Vector3Normalize(normal);
}

float Plane::distance(const Vector3& point) {
    return Vector3Dot((point - _vPosition), _vNormal);
}

}

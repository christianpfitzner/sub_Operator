#include "Ax12.h"

#include "DynamixelBus.h"

#include <iostream>
#include <cmath>

#define MSG(x) (std::cout << __PRETTY_FUNCTION__ << ": " << (x) << std::endl)

Ax12::Ax12(const int id, DynamixelBus* bus)
: Dynamixel(AX12, id, bus)
{
   MSG("what");

    if (!_bus) return;

    uint16_t value;

    if (!_bus->readWord(_id, CWAngleLimitL, value))
    {
        MSG("can't read minimum angle.");
        return;
    }
    _minPosition = value;

    if (!_bus->readWord(_id, CCWAngleLimitL, value))
    {
        MSG("can't read maximum angle.");
        return;
    }
    _maxPosition = value;
}

float Ax12::speed(void)
{
    if (!_bus)
    {
        MSG("no set bus.");
        return 0.0f;
    }

    uint16_t value;

    if (!_bus->readWord(_id, PresentSpeedL, value))
    {
        MSG("can't read speed.");
        return 0.0f;
    }

    /* check if speed is negative. */
    if (value & 0x400)
    {
        /* the negative value is not a compliment. */
        value &= 0x3ff;
        value *= -1;
    }

    return static_cast<float>(static_cast<int16_t>(value)) * 0.11f;
}

uint16_t Ax12::position(void)
{
    if (!_bus)
    {
        MSG("no set bus.");
        return 0.0f;
    }

    uint16_t value;

    if (!_bus->readWord(_id, PresentPositionL, value))
    {
        MSG("can't read position.");
        return 0.0f;
    }

    return value;
}

float Ax12::angle(void)
{
    uint16_t value = this->position();

    const float angle = static_cast<float>(static_cast<int>(value) - static_cast<int>(_startPosition)) * M_PI / 2000.0f;

    return angle;
}

float Ax12::load(void)
{
    if (!_bus)
    {
        MSG("no set bus.");
        return 0.0f;
    }

    uint16_t value;

    if (!_bus->readWord(_id, PresentLoadL, value))
    {
        MSG("can't read load.");
        return 0.0f;
    }

    /* check if load is negative */
    if (value & 0x400)
    {
        value &= 0x3ff;
        value *= -1;
    }

    return static_cast<float>(static_cast<int16_t>(value)) * 0.001f;
}

float Ax12::temperature(void)
{
    if (!_bus)
    {
        MSG("no set bus.");
        return 0.0f;
    }

    uint8_t value;

    if (!_bus->readByte(_id, PresentTemperature, value))
    {
        MSG("can't read temperature.");
        return 0.0f;
    }

    return static_cast<float>(value);
}

float Ax12::voltage(void)
{
    if (!_bus)
    {
        MSG("no set bus.");
        return 0.0f;
    }

    uint8_t value;

    if (!_bus->readByte(_id, PresentVoltage, value))
    {
        MSG("can't read voltage.");
        return 0.0f;
    }

    return static_cast<float>(value) * 0.1f;
}

float Ax12::current(void)
{
    if (!_bus)
    {
        MSG("no set bus.");
        return false;
    }

    uint16_t value;

    if (!_bus->readWord(_id, CurrentL, value))
    {
        MSG("can't read current.");
        return false;
    }

    return static_cast<float>(static_cast<int16_t>(value) - 2048) * 4.5e-3;
}

bool Ax12::setSpeed(const float speed)
{
    if (!_bus)
    {
        MSG("no set bus.");
        return false;
    }

    if (!_bus->writeWord(_id, MovingSpeedL, static_cast<uint16_t>(std::ceil(speed / 0.114f)) & 0x3ff))
    {
        MSG("can't set speed.");
        return false;
    }

    return true;
}

bool Ax12::setTorque(const float torque)
{
    if (!_bus)
    {
        MSG("no set bus.");
        return false;
    }

    if (torque < 0 || torque > 1)
    {
        MSG("torque is not in range.");
        return false;
    }

    if (!_bus->writeWord(_id, TorqueLimitL, static_cast<uint16_t>(torque * 1000.0f)))
    {
        MSG("can't set torque.");
        return false;
    }

    return true;
}

bool Ax12::setAngle(const float angle)
{
    if (!_bus)
    {
        MSG("no set bus.");
        return false;
    }

    if (angle < -2.0f * M_PI || angle > 2.0f * M_PI)
    {
        MSG("angle not in range.");
        return false;
    }

    const float posAngle = angle; //  < 0 ? 2 * M_PI + angle : angle;
    const uint16_t position = (static_cast<uint16_t>(posAngle / 0.005113118) + _startPosition ) & 0x03ff;

//    if(_id  == 2)
//    {
//
//      std::cout << "id: " << _id << "posAngle: " << posAngle << std::endl;
//      std::cout << "id: " << _id << "position: " << position << std::endl;
//    }

    if (!_bus->writeWord(_id, GoalPositionL, position))
    {
        MSG("can't set position.");
        return false;
    }

    return true;
}

bool Ax12::setMinimumPosition(const uint16_t position)
{
    if (!_bus)
    {
        MSG("no set bus.");
        return false;
    }

    if (position > 1024)
    {
        MSG("position is out of range.");
        return false;
    }

    if (!_bus->writeWord(_id, CWAngleLimitL, position))
    {
        MSG("can't write minimum position.");
        return false;
    }

    return true;
}

bool Ax12::setMaximumPosition(const uint16_t position)
{
    if (!_bus)
    {
        MSG("no set bus.");
        return false;
    }

    if (position > 1024)
    {
        MSG("position is out of range.");
        return false;
    }

    if (!_bus->writeWord(_id, CCWAngleLimitL, position))
    {
        MSG("can't write maximum position.");
        return false;
    }

    return true;
}

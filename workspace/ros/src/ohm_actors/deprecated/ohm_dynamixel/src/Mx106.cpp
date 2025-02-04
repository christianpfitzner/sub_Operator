#include "Mx106.h"

#include "DynamixelBus.h"

#include <iostream>
#include <cmath>

#define MSG(x) (std::cout << __PRETTY_FUNCTION__ << ": " << (x) << std::endl)

Mx106::Mx106(const int id, DynamixelBus* bus)
: Dynamixel(MX106, id, bus)
{
    if (!_bus)
        return;

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

float Mx106::speed(void)
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

uint16_t Mx106::position(void)
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

float Mx106::angle(void)
{
    uint16_t value = this->position();

    const float angle = static_cast<float>(static_cast<int>(value) - static_cast<int>(_startPosition))
        * M_PI / 2000.0f;

    return angle;
}

float Mx106::load(void)
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

float Mx106::temperature(void)
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

float Mx106::voltage(void)
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

float Mx106::current(void)
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

bool Mx106::setSpeed(const float speed)
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

bool Mx106::setTorque(const float torque)
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

bool Mx106::setAngle(const float angle)
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

    const float posAngle = angle < 0 ? 2 * M_PI + angle : angle;
    const uint16_t position = (static_cast<uint16_t>(posAngle / 0.001535111) + _startPosition) & 0x0fff;

    if (!_bus->writeWord(_id, GoalPositionL, position))
    {
        MSG("can't set position.");
        return false;
    }

    return true;
}

bool Mx106::setMinimumPosition(const uint16_t position)
{
    if (!_bus)
    {
        MSG("no set bus.");
        return false;
    }

    if (position > 4094)
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

bool Mx106::setMaximumPosition(const uint16_t position)
{
    if (!_bus)
    {
        MSG("no set bus.");
        return false;
    }

    if (position > 4094)
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

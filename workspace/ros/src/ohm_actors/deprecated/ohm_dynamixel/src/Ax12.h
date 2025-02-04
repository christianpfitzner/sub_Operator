#ifndef __AX12_H__
#define __AX12_H__

#include "Dynamixel.h"

class Ax12 : public Dynamixel
{
public:
    Ax12(const int id, DynamixelBus* bus = 0);

    virtual float speed(void);
    virtual uint16_t position(void);
    virtual float angle(void);
    virtual float load(void);
    virtual float temperature(void);
    virtual float voltage(void);
    virtual float current(void);

    virtual bool setSpeed(const float speed);
    virtual bool setTorque(const float torque);
    virtual bool setAngle(const float angle);
    virtual bool setMinimumPosition(const uint16_t position);
    virtual bool setMaximumPosition(const uint16_t position);

private:
    enum MemoryMap {
        ModelNumberL            = 0x00,
        ModelNumberH            = 0x01,
        VersionofFirmware       = 0x02,
        Id                      = 0x03,
        BaudRate                = 0x04,
        ReturnDelayTime         = 0x05,
        CWAngleLimitL           = 0x06,
        CWAngleLimitH           = 0x07,
        CCWAngleLimitL          = 0x08,
        CCWAngleLimitH          = 0x09,
        HighestLimitTemperature = 0x0b,
        LowestLimitVoltage      = 0x0c,
        HighestLimitVoltage     = 0x0d,
        MaxTorqueL              = 0x0e,
        MaxTorqueH              = 0x0f,
        StatusReturnLevel       = 0x10,
        AlarmLED                = 0x11,
        AlarmShutdown           = 0x12,
        TorqueEnable            = 0x18,
        LED                     = 0x19,

        CWComplianceMargin      = 0x1a,
        CCWComplianceMargin     = 0x1b,
        CWComplianceSlope       = 0x1c,
        CCWComplianceSlope      = 0x1d,

        GoalPositionL           = 0x1e,
        GoalPositionH           = 0x1f,
        MovingSpeedL            = 0x20,
        MovingSpeedH            = 0x21,
        TorqueLimitL            = 0x22,
        TorqueLimitH            = 0x23,
        PresentPositionL        = 0x24,
        PresentPositionH        = 0x25,
        PresentSpeedL           = 0x26,
        PresentSpeedH           = 0x27,
        PresentLoadL            = 0x28,
        PresentLoadH            = 0x29,
        PresentVoltage          = 0x2a,
        PresentTemperature      = 0x2b,
        Registered              = 0x2c,
        Moving                  = 0x2e,
        Lock                    = 0x2f,
        PunchL                  = 0x30,
        PunchH                  = 0x31,
        CurrentL                = 0x44,
        CurrentH                = 0x45,
        GoalAcceleration        = 0x49
    };
};

#endif

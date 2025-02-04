#ifndef __DYNAMIXEL_BUS_H__
#define __DYNAMIXEL_BUS_H__

#include <string>
#include <vector>
#include <stdint.h>
#include <tinyxml2.h>

class Dynamixel;

class DynamixelBus
{
public:
    DynamixelBus(const std::string& device);
    DynamixelBus(tinyxml2::XMLNode* node);
    ~DynamixelBus(void);

    bool readByte(const int id, const int address, uint8_t& value);
    bool readWord(const int id, const int address, uint16_t& value);
    bool writeWord(const int id, const int address, const uint16_t value);

    void getAllDevices(std::vector<Dynamixel*>& devices);
    Dynamixel* createDevice(tinyxml2::XMLElement* element);

private:
    Dynamixel* createDevice(const unsigned int id, const uint16_t modelCode);

    const std::string _deviceName;
};

#endif

#include "DynamixelBus.h"
#include "dynamixel.h"
#include "Mx106.h"
#include "Mx28.h"
#include "Ax12.h"

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <tinyxml2.h>

#define MSG(x) (std::cout << __PRETTY_FUNCTION__ << ": " << x << std::endl)

DynamixelBus::DynamixelBus(const std::string& device)
    : _deviceName(device)
{
    if (!dxl_initialize(device.c_str(), 57600))
    {
        std::cout << __PRETTY_FUNCTION__ << "can't open device " << device << "." << std::endl;
        return;
    }
}

DynamixelBus::DynamixelBus(tinyxml2::XMLNode* node)
{
    const tinyxml2::XMLElement* element = node->ToElement();

    if (!element || element->Name() != std::string("config"))
    {
        std::cout << __PRETTY_FUNCTION__ << ": not the expected node." << std::endl;
        return;
    }

    unsigned int baudrate = element->UnsignedAttribute("baudrate");
    std::string port = element->Attribute("port");

    if (!dxl_initialize(port.c_str(), baudrate))
    {
        std::cout << __PRETTY_FUNCTION__ << "can't open device " << port << "." << std::endl;
        return;
    }
}

DynamixelBus::~DynamixelBus(void)
{
    dxl_terminate();
}

bool DynamixelBus::readByte(const int id, const int address, uint8_t& value)
{
    for (int i = 0; i < 3; i++)
    {
        value = dxl_read_byte(id, address);
        if (dxl_get_result() == COMM_RXSUCCESS)
            return true;
    }

    return false;
}

bool DynamixelBus::readWord(const int id, const int address, uint16_t& value)
{
    for (int i = 0; i < 3; i++)
    {
        value = dxl_read_word(id, address);
        if (dxl_get_result() == COMM_RXSUCCESS)
            return true;
    }

    return false;
}

bool DynamixelBus::writeWord(const int id, const int address, const uint16_t value)
{
    dxl_write_word(id, address, value);
//    return dxl_get_result() == COMM_TXSUCCESS;
    return true;
}

void DynamixelBus::getAllDevices(std::vector<Dynamixel*>& devices)
{
    std::cout << "get all devices from bus " << _deviceName << "." << std::endl;

    for (int id = 0; id < 254; id++)
    {
        const uint16_t model = dxl_read_word(id, 0);

        if (dxl_get_result() != COMM_RXSUCCESS)
            continue;

        std::cout << "found device ";
        Dynamixel* device = this->createDevice(id, model);

        if (!device)
            continue;

        devices.push_back(device);
        std::cout << Dynamixel::modelName(device->model());
        std::cout << std::hex << " (0x" << model << ") with id " << std::dec << id << std::endl;
    }

    std::cout << "finished." << std::endl;
}

Dynamixel* DynamixelBus::createDevice(tinyxml2::XMLElement* element)
{
    if (element->Name() != std::string("servo"))
    {
        MSG("tag has not the name \"servo\".");
        return 0;
    }

    unsigned int id = element->UnsignedAttribute("id");

    if (!id || id > 254)
    {
        MSG("id " << id << " is out of range. (0 < id < 255)");
        return 0;
    }

    uint16_t model;
    this->readWord(id, 0, model);

    MSG("ModelNumber: " << model);

    if (!model)
    {
        MSG("no valid model number.");
        return 0;
    }

    Dynamixel* device = this->createDevice(id, model);

    if (!device)
    {
        MSG("can't create device with id " << id);
        return 0;
    }

    for (tinyxml2::XMLNode* node = element->FirstChild(); node ; node = node->NextSibling())
    {
        tinyxml2::XMLElement* childElement = node->ToElement();

        if (!childElement)
            continue;

        if (childElement->Name() == std::string("min-pos"))
        {
            device->setMinimumPosition(std::atoi(childElement->GetText()));
        }
        else if (childElement->Name() == std::string("max-pos"))
        {
            device->setMaximumPosition(std::atoi(childElement->GetText()));
        }
        else if (childElement->Name() == std::string("mid-pos"))
        {
            device->setStartPosition(std::atoi(childElement->GetText()));
        }
        else if (childElement->Name() == std::string("torque"))
        {
            device->setTorque(std::atof(childElement->GetText()));
        }
        else if (childElement->Name() == std::string("speed"))
        {
            device->setSpeed(std::atof(childElement->GetText()));
        }
    }

    return device;
}

Dynamixel* DynamixelBus::createDevice(const unsigned int id, const uint16_t modelCode)
{
    switch (modelCode)
    {
    case 0x140: //Mx106
        return new Mx106(id, this);

    case 0x1d: //Mx28
        return new Mx28(id, this);

    case 0xc: // Ax12
        return new Ax12(id, this);

    default:
        std::cout << "model unkown: " << std::hex << "0x" << modelCode << std::dec << std::endl;
        return 0;
    }
}

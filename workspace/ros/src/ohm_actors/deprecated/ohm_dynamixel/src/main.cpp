#include "DynamixelBus.h"
#include "Mx106.h"
#include "Ax12.h"

#include <iostream>
#include <cmath>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "usage: " << argv[0] << " <device>" << std::endl;
        return 1;
    }

    DynamixelBus bus(argv[1]);
    std::vector<Dynamixel*> devices;
    bus.getAllDevices(devices);

    std::cout << "Devices Size: " << devices.size() << std::endl;

    Ax12 jointA(2, &bus);
    Ax12 jointB(3, &bus);

    jointA.setStartPosition(1024);
    jointA.setSpeed(2.0f);
    jointA.setTorque(0.5f);
    jointA.setAngle(M_PI * 0.5f);

    jointB.setStartPosition(2080);
    jointB.setSpeed(2.0f);
    jointB.setTorque(0.5f);
    jointB.setAngle(M_PI * 0.9f);

    std::cout << "speed = " << jointA.speed()       << std::endl;
    std::cout << "angle = " << jointA.angle()       << std::endl;
    std::cout << "temp  = " << jointA.temperature() << std::endl;
    std::cout << "volt  = " << jointA.voltage()     << std::endl;
    std::cout << "load  = " << jointA.load()        << std::endl;
}

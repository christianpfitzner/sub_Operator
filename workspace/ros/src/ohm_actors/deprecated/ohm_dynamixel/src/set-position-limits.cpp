#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <sys/poll.h>

#include "DynamixelBus.h"
#include "Dynamixel.h"

const char* SPLITTER = "++++++++++++++++++++++++++++++++++++++++++++++++++++++";

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "usage: " << argv[0] << "<usb2dynamixel>" << std::endl;
        return 1;
    }

    DynamixelBus bus(argv[1]);
    std::vector<Dynamixel*> devices;

    std::cout << SPLITTER << std::endl;
    bus.getAllDevices(devices);
    std::cout << SPLITTER << std::endl << std::endl;

    if (!devices.size())
    {
        std::cout << "no devices found. will exit." << std::endl;
        return 1;
    }

    std::cout << "Select one of the following devices to set the position limits." << std::endl;

    for (int i = 0; i < devices.size(); i++)
    {
        std::cout << "(" << i + 1 << ") " << Dynamixel::modelName(devices[i]->model())
                  << ": id = " << devices[i]->id() << std::endl;
    }



    /* select a device */
    int device;

    do
    {
        std::cout << "select a device: ";
        std::cin >> device;
        device--;
    }
    while (device < 0 || device >= devices.size());


    /* get minimum position */
    std::cout << std::endl;
    std::cout << SPLITTER << std::endl;

    int position;

    for (int i = 1000; i >= 0; i--)
    {
        position = devices[device]->position();
        std::cout << "\rposition = " <<  position
                  << " take as minimum in " << static_cast<float>(i) * 0.01 << "s" << std::flush;

        ::usleep(10000);
    }

    devices[device]->setMinimumPosition(position);
    ::usleep(200000);

    /* get maximum position */
    std::cout << std::endl;
    std::cout << SPLITTER << std::endl;

    for (int i = 1000; i >= 0; i--)
    {
        position = devices[device]->position();
        std::cout << "\rposition = " <<  position
                  << " take as maximum in " << static_cast<float>(i) * 0.01 << "s" << std::flush;

        ::usleep(10000);
    }

    devices[device]->setMaximumPosition(position);
}

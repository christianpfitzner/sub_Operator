#include <iostream>
#include <string>
#include <vector>

#include <tinyxml2.h>
#include <ros/ros.h>
#include <ros/console.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Float32.h>
#include <ohm_actors_msgs/Status.h>
#include <ohm_actors_msgs/SetParameter.h>


#include "Dynamixel.h"
#include "DynamixelBus.h"

class DynamixelCallback
{
public:
    DynamixelCallback(Dynamixel* device) : _device(device) { }

    void callbackCommand(const std_msgs::Float64& msg)
    {
        _device->setAngle(msg.data);
    }

    bool callbackTorque(ohm_actors_msgs::SetParameter::Request& req, ohm_actors_msgs::SetParameter::Response& res)
    {
        _device->setTorque(req.value);
        return true;
    }

    bool callbackSpeed(ohm_actors_msgs::SetParameter::Request& req, ohm_actors_msgs::SetParameter::Response& res)
    {
      std::cout << __PRETTY_FUNCTION__ << "here" << std::endl;
        _device->setSpeed(req.value);
        return true;
    }

    void callbackSpeed_sub(const std_msgs::Float32& msg)
    {
       _device->setSpeed(msg.data);
    }

private:
    Dynamixel* _device;
};

std::vector<Dynamixel*> _devices;
std::vector<DynamixelCallback> _callbacks;
std::vector<std::string> _topics;
DynamixelBus* _bus = 0;
unsigned int _rate = 10;

void configureBus(const std::string& xmlFile)
{
    tinyxml2::XMLDocument doc;

    doc.LoadFile(xmlFile.c_str());
    tinyxml2::XMLNode* rootNode = doc.FirstChild();

    if (!rootNode)
    {
        ROS_ERROR("no node in xml file.");
        return;
    }


    for (tinyxml2::XMLNode* node = rootNode->NextSibling()->FirstChild(); node; node = node->NextSibling())
    {
        tinyxml2::XMLElement* element = node->ToElement();

        if (!element)
            continue;

        if (element->Name() == std::string("config")) // this is not good! because the bus must comes first.
        {
            _bus = new DynamixelBus(node);
            _rate = element->UnsignedAttribute("rate") ? element->UnsignedAttribute("rate") : 20;

        }
        else if (element->Name() == std::string("servo") && _bus) // also here just a fast hack!
        {
            Dynamixel* device = _bus->createDevice(element);
            std::string topic = element->Attribute("topic");


            if (!device || topic.empty()) {
                ROS_INFO("device not vaild or no topic set.");
                continue;
            }

            _devices.push_back(device);
            _callbacks.push_back(device);
            _topics.push_back(topic);

            ROS_INFO_STREAM("added device " << Dynamixel::modelName(device->model())
                         << " with id " << device->id());
        }
    }
}

void createTopics(ros::NodeHandle& nh,
                  std::vector<ros::Subscriber>& subscribers,
                  std::vector<ros::Subscriber>& speedSubs,
                  std::vector<ros::Publisher>& publishers,
                  std::vector<ros::ServiceServer>& torqueServers,
                  std::vector<ros::ServiceServer>& speedServers)
{
    subscribers.resize(  _devices.size());
    speedSubs.resize(    _devices.size());
    publishers.resize(   _devices.size());
    torqueServers.resize(_devices.size());
    speedServers.resize( _devices.size());

    for (unsigned int i = 0; i < _devices.size(); i++)
    {
        subscribers[i]   = nh.subscribe(_topics[i] + "/command", 2,
                                              &DynamixelCallback::callbackCommand, &_callbacks[i]);
        publishers[i]    = nh.advertise<ohm_actors_msgs::Status>(_topics[i] + "/status", 1);
        torqueServers[i] = nh.advertiseService(_topics[i] + "/torque",
                                              &DynamixelCallback::callbackTorque,
                                              &_callbacks[i]);
        speedServers[i]  = nh.advertiseService(_topics[i] + "/speed",
                                              &DynamixelCallback::callbackSpeed,
                                              &_callbacks[i]);
        speedSubs[i]     = nh.subscribe(_topics[i] + "/speed", 2,
                                        &DynamixelCallback::callbackSpeed_sub,
                                        &_callbacks[i]);

    }
}


int main(int argc, char** argv)
{
    ros::init(argc, argv, "dynamixel_server");
    ros::NodeHandle para("~");
    std::string xmlFile;
    para.param<std::string>("xml", xmlFile, "bus.xml");

    configureBus(xmlFile);

    ros::NodeHandle nh;
    std::vector<ros::Subscriber>    subscribers;
    std::vector<ros::Subscriber>    speedSubcribers;
    std::vector<ros::Publisher>     publishers;
    std::vector<ros::ServiceServer> torqueServers;
    std::vector<ros::ServiceServer> speedServers;
    ros::Rate rate(_rate);
    static unsigned int seq = 0;

    createTopics(nh, subscribers, speedSubcribers, publishers, torqueServers, speedServers);

    while (ros::ok())
    {
        ++seq;

        for (unsigned int i = 0; i < _devices.size(); i++)
        {
//            ohm_actors_msgs::Status status;
//
//            status.header.seq    = seq;
//            status.header.stamp  = ros::Time::now();
//            status.id            = _devices[i]->id();
//            status.type          = Dynamixel::modelName(_devices[i]->model());
//            status.angle         = _devices[i]->angle();
//            status.speed         = _devices[i]->speed();
//            status.load          = _devices[i]->load();
//            status.voltage       = _devices[i]->voltage();
//            status.current       = _devices[i]->current();
//            status.temperature   = _devices[i]->temperature();
//            status.position      = _devices[i]->position();
//
//            publishers[i].publish(status);
        }
        ros::spinOnce();
        rate.sleep();
    }
}

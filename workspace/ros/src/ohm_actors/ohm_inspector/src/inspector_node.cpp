/*
 * inspector_node.cpp
 *
 *  Created on: 19.03.2015
 *      Author: feesma44884
 */
#include "inspector_node.hpp"

InspectorNode::InspectorNode(const std::string& topicName, const std::string& serialPort, const uint16_t& timeoutMS,
                             const uint8_t& maxErrorCnt, const double& updateFrequencyHz,
                             const float& speed2DScale, const float& speedAxisScale) :
_topicName(topicName),
_updateFrequency(updateFrequencyHz),
_loopRate(updateFrequencyHz),
_speed2DScale(speed2DScale),
_speedAxisScale(speedAxisScale),
_maxErrorCnt(maxErrorCnt),
_nodeHandle(),
_cmdSubscriber(),
_resetSubscriber(),
_openCMInterface(timeoutMS, serialPort),
_commandData(),
_newCommandReceived(false),
_comState(CS_SEND_COMMAND_DATA),
_angles(),
_inspectorState(STATE_PARKED),
_tfInspector(),
_tfBroadcaster()
{
    ROS_INFO("Create InspectorNode: Port: %s TimeoutMS: %i MaxErrorCnt: %i "
            "updateFrequencyHz: %.2f speed2DScale: %.2f speedAxisScale: %.2f",
             serialPort.c_str(), timeoutMS, _maxErrorCnt, _updateFrequency, _speed2DScale, _speedAxisScale);

    //check if loop rate has smaller time as timeoutMS -> if not the board will always disconnect
    if((1.0 / updateFrequencyHz) > timeoutMS * 0.001f) {
        ROS_ERROR("Update frequency is too slow. Timeout will always occure! Increase update frequency or timeout!");
    }

    //subscribe command messages
    _cmdSubscriber = _nodeHandle.subscribe<ohm_actors_msgs::Command>("/" + _topicName, 30, &InspectorNode::commandCallback, this);

    //subscribe reset messages
    _resetSubscriber = _nodeHandle.subscribe<std_msgs::Bool>("/" + _topicName + "/reset", 40, &InspectorNode::resetCallback, this);

    //publisher
    _statePublisher =   _nodeHandle.advertise<std_msgs::String>("/" + _topicName + "/state", 1);

    //reset vars
    for(uint8_t i = 0; i < 4; i++) {
        _angles[i]  =   0.0f;
    }

    //reset all transform
    for(uint8_t i = 0; i < 7; i++)
        _tfInspector[i].setIdentity();

    //setup transform
    tf::Quaternion  quat;

    //base to base link
    _tfInspector[0].setOrigin(tf::Vector3(0.0f, 0.0f, INSPECTOR_L_0_M));

    //base link to first joint
    _tfInspector[1].setIdentity();

    //first joint to first link
    quat.setRPY(0.0f, 0.0f, -M_PI * 0.5f);
    _tfInspector[2].setOrigin(tf::Vector3(INSPECTOR_L_1_M, 0.0f, 0.0f));
    _tfInspector[2].setRotation(quat);

    //first link to second joint
    _tfInspector[3].setOrigin(tf::Vector3(INSPECTOR_L_2_M, 0.0f, 0.0f));

    //second joint to third joint
    _tfInspector[4].setOrigin(tf::Vector3(INSPECTOR_L_3_M, 0.0f, 0.0f));

    //third joint to fourth joint
    _tfInspector[5].setOrigin(tf::Vector3(INSPECTOR_L_4_M, 0.0f, 0.0f));

    //fourth joint to TCP
    _tfInspector[6].setOrigin(tf::Vector3(INSPECTOR_L_5_M, 0.0f, 0.0f));
}

void InspectorNode::runNode() {

    //run node as long as ros is ok
    while(ros::ok()) {
        //vars
        bool result = true;

        //wait
        if(!wait(500)) return;

        //try to connect
        if(!connect())  return;

        //infinite message loop
        while(ros::ok() && result == true) {

            //run application
            result = messageLoop();

            //process messages
            ros::spinOnce();

            //wait
            _loopRate.sleep();
        }

    };
}

bool InspectorNode::connect() {
    unsigned int errorID    =   0;

    //try to connect until connection is established or abort by user
    while(ros::ok()) {
        if(_openCMInterface.connect(errorID) == false) {

            //check if error is caused from dynamixel drives
            if(errorID != 0) {
                ROS_WARN("InspectorNode::connect(): OpenCM board refused connection, "
                        "because the dynamixel drive with ID %i has an error!\nRetry after 2 seconds.", errorID);

                //publish error to state
                std_msgs::String          stateMsg;
                std::stringstream         stateStr;

                stateStr    << "STATE_ERROR_DYNAMIXEL_" << errorID;
                stateMsg.data = stateStr.str();
                _statePublisher.publish(stateMsg);
            }
            else {
                ROS_WARN("InspectorNode::connect(): Can't establish connection to OpenCM9.04 board @%s\n "
                                     "Check if board is connected and you've access rights to the port!\n"
                                     "Retry after 2 second!", _openCMInterface.getDeviceName().c_str());
            }


            if(!wait(2000)) return false;
        }
        else {
            ROS_INFO("Connected to OpenCM9.04 board @ %s", _openCMInterface.getDeviceName().c_str());

            //reset vars
            _comState = CS_SEND_COMMAND_DATA;

            return true;
        }
    }

    return false;
}

bool InspectorNode::messageLoop() {

    static uint8_t cnt  =   0;

    //process messages
    processMessages();

    //do communication depending on current state
    switch(_comState) {
    case CS_SEND_COMMAND_DATA:
    {
        if(!sendCommandData())  return false;

        //check when next state will come
        if(cnt >= 5) {
            _comState = CS_RECEIVE_ANGLE_DATA;
            cnt = 0;
        }
        else {
            cnt++;
        }
    }break;
    case CS_RECEIVE_ANGLE_DATA:
    {
        if(!receiveAngleData()) return false;

        //nex state
        _comState = CS_SEND_COMMAND_DATA;
    }break;
    }

    //broadcast transform
    broadcastTransform();

    //keep connection established
    if(!_openCMInterface.keepConnection()) {
        return false;
    }

    return true;
}

void InspectorNode::processMessages() {
    static ros::Time   lastMessageReceivedTimeStamp    =   ros::Time::now();

    //check if new command was received
    if(_newCommandReceived) {
        //reset var
        lastMessageReceivedTimeStamp    =   ros::Time::now();

        //scale speed
        _commandData.speed2D[0]         *=  _speed2DScale;
        _commandData.speed2D[1]         *=  _speed2DScale;
        _commandData.speedAxis[0]       *=  _speedAxisScale;
        _commandData.speedAxis[1]       *=  _speedAxisScale;

        //check if speed2D is 0.0
        if((_commandData.speed2D[0] + _commandData.speed2D[1]) == 0.0f) {
            //calculate speed depending on orientation of the TCP
            float tcpAngle = -1.0f * _angles[0] + _angles[1] - _angles[2];
            _commandData.speed2D[0] =   cosf(tcpAngle) * _commandData.speedTCP * _speed2DScale;
            _commandData.speed2D[1] =   sinf(tcpAngle) * _commandData.speedTCP * _speed2DScale;
        }


        _newCommandReceived             =   false;
    }
    //if no command was received for timeoutMS all commands are reseted
    else if(((ros::Time::now().toSec() - lastMessageReceivedTimeStamp.toSec()) * 1000.0f) >
            static_cast<float>(_openCMInterface.getTimeoutMS())) {
        //reset vars
        _commandData                    =   ohm_actors_msgs::Command();
        lastMessageReceivedTimeStamp    =   ros::Time::now();

        //throw warning
        ROS_WARN("No commands received until %i milliseconds! All movements are stopped!", _openCMInterface.getTimeoutMS());
    }
}

bool InspectorNode::sendCommandData() {
    static bool     moveHome = false;
    static bool     movePark = false;

    //check if move home
    if(_commandData.moveHome == true && moveHome == false) {
        moveHome = true;
        _openCMInterface.sendMoveHomeMessage();
    }
    else if(_commandData.moveHome == false && moveHome == true) {
        moveHome = false;
        _openCMInterface.sendStopMessage();
    }

    //check if move park
    if(_commandData.movePark == true && movePark == false) {
        movePark = true;
        _openCMInterface.sendMoveParkMessage();
    }
    else if(_commandData.movePark == false && movePark == true) {
        movePark = false;
        _openCMInterface.sendStopMessage();
    }

    //check if continous path movement
    if(_commandData.moveHome == false && _commandData.movePark == false) {
        if(!_openCMInterface.sendMoveCPMessage(_commandData.speed2D[0], _commandData.speed2D[1],
                                               _commandData.speedAxis[0], _commandData.speedAxis[1]))
        {
            return false;
        }
    }

    return true;
}

bool InspectorNode::receiveAngleData() {

    //get angle data
    if(!_openCMInterface.getCurrentAngles(_angles, _inspectorState))
        return false;

    std_msgs::String          stateMsg;
    std::stringstream         stateStr;

    //show state
    switch(_inspectorState) {
    case STATE_PARKED:
    {
      ROS_INFO("STATE_PARKED");
      stateStr    <<  "STATE_PARKED";
    }break;
    case STATE_RELEASED:
    {
      ROS_INFO("STATE_RELEASED");
      stateStr    <<  "STATE_RELEASED";
    }break;
    case STATE_MOVE_HOME:
    {
      ROS_INFO("STATE_MOVE_HOME");
      stateStr    <<  "STATE_MOVE_HOME";
    }break;
    case STATE_MOVE_PARK:
    {
      ROS_INFO("STATE_MOVE_PARK");
      stateStr    <<  "STATE_MOVE_PARK";
    }break;
    }

    stateMsg.data   =   stateStr.str();
    _statePublisher.publish(stateMsg);

    return true;
}

void InspectorNode::commandCallback(const ohm_actors_msgs::Command::ConstPtr& command) {
    _commandData        =   (*command);
    _newCommandReceived =   true;
}

void InspectorNode::resetCallback(const std_msgs::Bool::ConstPtr& data) {
    //check data
    if((*data).data == true) {
        //send reset message if connected
        _openCMInterface.sendDynamixelResetMessage();

        ROS_WARN("Send reset request for dynamixel drives!");
    }

}

void InspectorNode::broadcastTransform() {
    tf::Quaternion  quaternion;

//    //send base -> base link
    quaternion.setRPY(0.0f, -M_PI * 0.5f, 0.0f);
    _tfInspector[0].setRotation(quaternion);
    _tfBroadcaster.sendTransform(tf::StampedTransform(_tfInspector[0], ros::Time::now(), "simon/base_link", _topicName + "/base_link" ));

    //base link -> first joint
    quaternion.setRPY(0.0f, 0.0f, -_angles[0]);
    _tfInspector[1].setRotation(quaternion);
    _tfBroadcaster.sendTransform(tf::StampedTransform(_tfInspector[1], ros::Time::now(), _topicName + "/base_link", _topicName + "/joint_1" ));

    //transform first joint -> first link
    _tfBroadcaster.sendTransform(tf::StampedTransform(_tfInspector[2], ros::Time::now(), _topicName + "/joint_1", _topicName + "/link_1"));

    //transform first link -> second joint
    quaternion.setRPY(0.0f, 0.0f, _angles[1]);
    _tfInspector[3].setRotation(quaternion);
    _tfBroadcaster.sendTransform(tf::StampedTransform(_tfInspector[3], ros::Time::now(), _topicName + "/link_1", _topicName + "/joint_2"));

    //transform second joint -> third joint
    quaternion.setRPY(0.0f, 0.0f, -_angles[2]);
    _tfInspector[4].setRotation(quaternion);
    _tfBroadcaster.sendTransform(tf::StampedTransform(_tfInspector[4], ros::Time::now(), _topicName + "/joint_2", _topicName + "/joint_3"));

    //transform third joint -> fourth joint
    quaternion.setRPY(-M_PI * 0.5f, _angles[3], 0.0f);
    _tfInspector[5].setRotation(quaternion);
    _tfBroadcaster.sendTransform(tf::StampedTransform(_tfInspector[5], ros::Time::now(), _topicName + "/joint_3", _topicName + "/joint_4"));

    //transform fourth joint -> TCP
    _tfBroadcaster.sendTransform(tf::StampedTransform(_tfInspector[6], ros::Time::now(), _topicName + "/joint_4", _topicName + "/TCP"));

}

bool InspectorNode::wait(const unsigned int& timeMS) {

    for(unsigned int i = 0; i < timeMS; i++) {
        usleep(1000);
        if(ros::ok() == false) return false;
    }

    return true;
}

/**
 * Main Function of the application.
 *
 * @param argc[in]: Number of arguments
 * @param argv[in]: Array with the argument data
 * @return
 */
int main(int argc, char** argv) {
    //vars
    std::string topicName, serialPort;
    int         timeoutMS, maxErrorCnt;
    double      updateFrequencyHz, speed2DScale, speedAxisScale;

    //init ros system
    ros::init(argc, argv, "ohm_inspector_node");

    //get parameters from startup
    ros::NodeHandle para("~");
    para.param<std::string>("topicName",         topicName, "ohm_inspector");
    para.param<std::string>("serialPort",        serialPort, "/dev/opencm_inspector");
    para.param<int>(        "timeoutMS",         timeoutMS, 100);
    para.param<int>(        "maxErrorCnt",       maxErrorCnt, 1);
    para.param<double>(     "updateFrequencyHz", updateFrequencyHz, 30.0);
    para.param<double>(     "speed2DScale",      speed2DScale, 100.0);
    para.param<double>(     "speedAxisScale",    speedAxisScale, 10.0);

    //create instance of node
    InspectorNode   inspector(topicName, serialPort, static_cast<uint16_t>(timeoutMS),
                              static_cast<uint8_t>(maxErrorCnt), updateFrequencyHz, speed2DScale, speedAxisScale);
    //run inspector node
    inspector.runNode();


    return 0;
}

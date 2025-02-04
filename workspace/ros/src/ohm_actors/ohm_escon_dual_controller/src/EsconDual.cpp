
#include "EsconDual.h"


namespace{

namespace serial{
const uint8_t TRIGGER = 0xff;

enum DATA_FORMAT{
    MOTOR_R_LOW = 0,
    MOTOR_R_HIGH,    //1 = forward, 0 = backward
    MOTOR_L_LOW,
    MOTOR_L_HIGH,    //1 = forward, 0 = backward
    CRC,
    DATA_SIZE
};
}

}

EsconDual::EsconDual() : _rate(0)
{
    _loopRate = 0;

    //rosParam
    ros::NodeHandle privNh("~");
    std::string sub_name;
    std::string arduino_device;
    bool use_stamped;
    double gear_ratio;
    double wheel_diameter;
    double wheelbase;
    double duration_max_no_cmd;
    int rpm_max;
    
    privNh.param("sub_name",sub_name,std::string("cmd_vel"));
    privNh.param("arduino_device",arduino_device,std::string("/dev/escon_controller"));
    privNh.param<bool>("use_stamped",use_stamped, false);
    privNh.param<double>("gear_ratio",gear_ratio,74.0);     // x/1   x = given vlaue
    privNh.param<double>("wheel_diameter",wheel_diameter,0.245); // in [m]
    privNh.param<double>("wheelbase",wheelbase,0.435);
    privNh.param<double>("duration_max_no_cmd", duration_max_no_cmd, 1); //value in [s]
    privNh.param<int>("rpm_max", rpm_max, 7580);


    _stamped_used = use_stamped;
    _factor_ms_rpm = (60 * gear_ratio) / (M_PI * wheel_diameter);
    _wheelbase = wheelbase;
    _rpm_max = rpm_max;

    _serialPort = new apps::SerialCom(arduino_device, apps::B_9600, apps::F_8N1);
    _serialData = new uint8_t[serial::DATA_SIZE];
    _checksum = new apps::CRC_8();

    _dur_max_no_cmd = ros::Duration(duration_max_no_cmd);

    //init publisher
    //_pub = _nh.advertise<ROS_PACK::MSG>("topicName",1);


    //inti subscriber
    if(_stamped_used)
    {
        _subTwist = _nh.subscribe(sub_name, 1, &EsconDual::subTwistStampedCallback, this);
        ROS_INFO("TWIST_STAMPED is used");
    }
    else
    {
        _subTwist = _nh.subscribe(sub_name, 1, &EsconDual::subTwistCallback, this);
        ROS_INFO("TWIST is used");
    }
}

EsconDual::~EsconDual()
{
    delete _rate;
}

void EsconDual::start(const unsigned int rate)
{
    delete _rate;
    _loopRate = rate;
    _rate = new ros::Rate(_loopRate);

    //open serialPort
    if(_serialPort->connect() != apps::DeviceSucces)
    {
        ROS_ERROR("Could not open Port %s ... will exit",_serialPort->getDeviceName().c_str());
        exit(-1);
    }

    this->run();
}

void EsconDual::run()
{
    //unsigned int cnt = 0;

    //ros::spin();

    while(ros::ok())
    {
        //do stuff;

        //publish data;
        //_pub.publish(msg);

       if((ros::Time::now() - _time_last_cmd) > _dur_max_no_cmd)
       {
          this->transmittData(0,0);
       }

       ros::spinOnce();
       _rate->sleep();
    }
}

void EsconDual::handleTwistData(double linear, double angular)
{
   //ROS_INFO("linear,angular : (%f,%f)",linear,angular);

    double linear_rpm = linear * _factor_ms_rpm;
    //prove abs max value
    if(std::abs(linear_rpm) > (double)_rpm_max)
    {
        linear_rpm = _rpm_max * (std::abs(linear_rpm)/linear_rpm);
    }
    //ROS_INFO("Linear_rpm: %f",linear_rpm);
    double delta_rpm = -1 * angular * _wheelbase * _factor_ms_rpm;
    if(std::abs(delta_rpm) > (double)_rpm_max*2)
    {
        linear_rpm = _rpm_max*2 * (std::abs(delta_rpm)/delta_rpm);
    }

    int16_t r = (uint16_t)(linear_rpm + 0.555) - (uint16_t)(delta_rpm/2 + 0.555);
    int16_t l = (uint16_t)(linear_rpm + 0.555) + (uint16_t)(delta_rpm/2 + 0.555);

   //ROS_INFO("right,left : (%d,%d)",r,l);
    this->transmittData(r,l);

}


void EsconDual::transmittData(int16_t r_rpm, int16_t l_rpm)
{
   int16_t r = r_rpm;
   int16_t l = l_rpm;

   //set serial Data
   uint8_t low = r & 0x00ff;
   uint8_t high = r>>8 & 0x00ff;
   _serialData[serial::MOTOR_R_LOW ] = low;
   _serialData[serial::MOTOR_R_HIGH] = high;
   low = l & 0x00ff;
   high = l>>8 & 0x00ff;
   _serialData[serial::MOTOR_L_LOW] = low;
   _serialData[serial::MOTOR_L_HIGH] = high;
   //for now set to 0 @todo compute crc
   _serialData[serial::CRC] = _checksum->computeCRC(_serialData,serial::DATA_SIZE-1);

  //"ROS_INFO("rh_rl, lh_ll: [%d%d, %d%d]",_serialData[serial::MOTOR_R_HIGH],
  //                           _serialData[serial::MOTOR_R_LOW],
  //                           _serialData[serial::MOTOR_L_HIGH],
  //                           _serialData[serial::MOTOR_L_LOW]);

   //transmitt to arduino
   if(_serialPort->transmit(serial::TRIGGER) != apps::DeviceSucces)
   {//error
       ROS_ERROR("Error at transmitting trigger to arduino");
       return;
   }
   for(unsigned int i = 0; i < serial::DATA_SIZE; i++)
   {
     //ROS_INFO("%d",_serialData[i]);
       if(_serialPort->transmit(_serialData[i]) != apps::DeviceSucces)
       {
           ROS_ERROR("Error at transmitting data to arduino");
           return;
       }
   }

   //ROS_INFO("Received: ");
//    uint8_t serial_tmp[serial::DATA_SIZE] = {0};
//    for(unsigned int i = 0; i < serial::DATA_SIZE; i++)
//    {
//       uint8_t tmp = 0;
//      _serialPort->receive(tmp,100000);
//      ROS_INFO("%d",tmp);
//    }

   uint8_t tmp_r = 0,tmp_l = 0;
   _serialPort->receive(tmp_r,100000);
   _serialPort->receive(tmp_l,100000);
   //ROS_INFO("pwm(r,l) : (%d,%d)",tmp_r,tmp_l);
}

void EsconDual::subTwistCallback(const geometry_msgs::Twist& msg)
{
    _time_last_cmd = ros::Time::now();
    this->handleTwistData(msg.linear.x, msg.angular.z);
}



void EsconDual::subTwistStampedCallback(const geometry_msgs::TwistStamped& msg)
{
    _time_last_cmd = ros::Time::now();
    //todo check is pkg is valid via ROS::Time
    this->handleTwistData(msg.twist.linear.x, msg.twist.angular.z);
}


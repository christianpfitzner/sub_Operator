#include "Motorcontroller.h"
#include <unistd.h>

#include "params.h"

#include <termios.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>

#include <iostream>
#include <string.h>
#include <stdlib.h>

#include <string>
#include <ros/ros.h>

using namespace std;

Motorcontroller::Motorcontroller()
{
	_rpmMax = RPMMAX;
	_cmdMax = CMDMAX;

	_maxCmd = CMDMAX;
	_minCmd = -CMDMAX;

	std::string comPort;
	ros::NodeHandle prvNh("~");
	prvNh.param<std::string>("com_port", comPort, "/dev/ttymxc3");

	const speed_t baud = B115200;
  _com = new SerialPort(comPort.c_str(), baud);
}

Motorcontroller::~Motorcontroller()
{
  stop();
  delete _com;
}

int Motorcontroller::getRPMMax()
{
  return (int)_rpmMax;
}

void Motorcontroller::setRPM(double rpm[6])
{
  // voltage control vector, i.e., [-u_max;u_max]
  unsigned char message[14];
  memset(message, 0, 14*sizeof(char));

  // normalized control vector in range [_minCmd; _maxCmd]
  double x[6];
  for(int i=0; i<6; i++)
  {
    x[i] = rpm[i]  / _rpmMax * _maxCmd;
    if(x[i]>_maxCmd) x[i] = _maxCmd;
    if(x[i]<_minCmd) x[i] = _minCmd;
  }

  message[0] = '#';
  for(int i=0; i<6; i++)
  {
    short sx = x[i];
    message[2*i+1] = (unsigned char)((0xFF00 & sx) >>8);
    message[2*i+2] = (unsigned char)(0x00FF & sx);
  }
  message[13] = '$';

  _com->send((char*)message, 14);

  short events[6];
  bool retval = _com->receive(events, 6);

  if(retval)
  {
    cout << "rpm: " << endl;
    for(int i=0; i<6; i++)
    {
      _rpm[i] = (double)events[i];
      printf("%05.0lf ", _rpm[i]);
    }
    cout << endl;
  }
}

void Motorcontroller::setRPM(double rpm1, double rpm2)
{
  // voltage control vector, i.e., [-u_max;u_max]
  unsigned char message[14];
  memset(message, 0, 14*sizeof(char));

  // normalized control vector in range [_minCmd; _maxCmd]
  double x[2];
  x[0] = rpm1  / _rpmMax * _maxCmd;
  if(x[0]>_maxCmd) x[0] = _maxCmd;
  if(x[0]<_minCmd) x[0] = _minCmd;
  x[1] = rpm2  / _rpmMax * _maxCmd;
  if(x[1]>_maxCmd) x[1] = _maxCmd;
  if(x[1]<_minCmd) x[1] = _minCmd;

  message[0] = '#';
  message[1] = (unsigned char)((0xFF00 & ((short)x[1])) >>8);
  message[2] = (unsigned char) (0x00FF & ((short)x[1]));
  message[3] = (unsigned char)((0xFF00 & ((short)x[2])) >>8);
  message[4] = (unsigned char) (0x00FF & ((short)x[2]));
  message[13] = '$';

  _com->send((char*)message, 14);

  short events[6];
  bool retval = _com->receive(events, 6);

  if(retval)
  {
    _rpm[0] = (double)events[0];
    printf("rpm: %05.0lf ", _rpm[0]);
    _rpm[1] = (double)events[1];
    printf("%05.0lf\n ", _rpm[1]);
  }
}

void Motorcontroller::getRPM(double rpm[6])
{
  for(int i=0; i<6; i++)
    rpm[i] = _rpm[i];
}

void Motorcontroller::getRPM(double &rpm1, double &rpm2)
{
    rpm1 = _rpm[0];
    rpm2 = _rpm[1];
}

void Motorcontroller::stop()
{
  double rpm[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  setRPM(rpm);
}

#ifndef MOTORCONTROLLER_H_
#define MOTORCONTROLLER_H_

#include "SerialPort.h"

/**
 * @class Motorcontroller
 * @author Stefan May
 * @date 13.11.2014
 */
class Motorcontroller
{

public:

  /**
   * Default constructor
   */
	Motorcontroller();

	/**
	 * Destructor
	 */
	virtual ~Motorcontroller();

	/**
	 * Get maximum revolutions per minute
	 * @return maximum rpm
	 */
	int getRPMMax();

	/**
   * Set revolutions per minute
   * @param rpm target rpm of each channel
   */
  void setRPM(double rpm[6]);
  void setRPM(double rpm1, double rpm2);

	/**
	 * Get revolutions per minute of all wheels
	 * @param rpm revolutions per minute of all channels
	 */
	void getRPM(double rpm[6]);
	void getRPM(double &rpm1, double &rpm2);

	/**
	 * Stop motors
	 */
  void stop();

private:

	double _cmdMax;
	double _rpmMax;

	int _maxCmd;
	int _minCmd;

	SerialPort* _com;

	double _rpm[6];
};

#endif /* MOTORCONTROLLER_H_ */

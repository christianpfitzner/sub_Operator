/*
 * ViscaSend.h
 *
 *  Created on: 03.01.2019
 *      Author: volletjo
 */

#ifndef VISCASEND_H_
#define VISCASEND_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <iostream>
#include "ViscaSerial.h"
#include <ohm_visca_control/viscaControl.h>

struct VISCAControlData_t
{
//zoom standard speed
bool teleStandard;
bool wideStandard;
//zoom variable Speed
bool teleVariable;
bool wideVariable;
uint8_t variableSpeed;
//direct zoom level
uint16_t zoomDirect;
//autofocus
bool autofocus;
uint16_t focusDirect;
//direct zoom and focus level
bool zoomFocusDirect;
};

class ViscaControlData
{
private:
  ohm_visca_control::viscaControl _old_control_data;
public:
  uint32_t send_control_data(VISCAInterface_t *iface, VISCACamera_t *camera, ohm_visca_control::viscaControl new_control_data);
};

extern ViscaControlData viscaSendControl;

#endif /* VISCASEND_H_ */

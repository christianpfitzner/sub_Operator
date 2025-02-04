/*
 * ViscaSend.cpp
 *
 *  Created on: 03.01.2019
 *      Author: volletjo
 */

#include "ViscaSend.h"


uint32_t ViscaControlData::send_control_data(VISCAInterface_t *iface, VISCACamera_t *camera, ohm_visca_control::viscaControl new_control_data)
{
  if(new_control_data.teleStandard != _old_control_data.teleStandard)
  {
    if(new_control_data.teleStandard == 1)
    {
      viscaSerial.set_zoom_tele(iface, camera);
    }
    else
    {
      viscaSerial.set_zoom_stop(iface, camera);
    }
  }

  if(new_control_data.wideStandard != _old_control_data.wideStandard)
  {
    if(new_control_data.wideStandard == 1)
    {
      viscaSerial.set_zoom_wide(iface, camera);
    }
    else
    {
      viscaSerial.set_zoom_stop(iface, camera);
    }
  }

  if((new_control_data.teleVariable != _old_control_data.teleVariable) || (new_control_data.wideVariable != _old_control_data.wideVariable) || (new_control_data.variableSpeed != _old_control_data.variableSpeed))
  {
    if(new_control_data.teleVariable == 1)
    {
      viscaSerial.set_zoom_tele_speed(iface, camera, new_control_data.variableSpeed);
    }
    else if(new_control_data.wideVariable == 1)
    {
      viscaSerial.set_zoom_wide_speed(iface, camera, new_control_data.variableSpeed);
    }
    else
    {
      viscaSerial.set_zoom_stop(iface, camera);
    }
  }

  if((new_control_data.zoomDirect != _old_control_data.zoomDirect) || (new_control_data.focusDirect != _old_control_data.focusDirect) || (new_control_data.zoomFocusDirect != _old_control_data.zoomFocusDirect))
  {
    if(new_control_data.zoomFocusDirect == 1)
    {
      viscaSerial.set_zoom_and_focus_direct(iface, camera, new_control_data.zoomDirect, new_control_data.focusDirect);
    }
    else if(new_control_data.zoomDirect != _old_control_data.zoomDirect)
    {
      viscaSerial.set_zoom_direct(iface, camera, new_control_data.zoomDirect);
    }
    else if(new_control_data.focusDirect != _old_control_data.focusDirect)
    {
      viscaSerial.set_focus_direct(iface, camera, new_control_data.focusDirect);
    }
  }

  if(new_control_data.farStandard != _old_control_data.farStandard)
  {
    if(new_control_data.farStandard == 1)
    {
      viscaSerial.set_focus_far(iface, camera);
    }
    else
    {
      viscaSerial.set_focus_stop(iface, camera);
    }
  }

  if(new_control_data.nearStandard != _old_control_data.nearStandard)
  {
    if(new_control_data.nearStandard == 1)
    {
      viscaSerial.set_focus_near(iface, camera);
    }
    else
    {
      viscaSerial.set_focus_stop(iface, camera);
    }
  }

  if((new_control_data.farVariable != _old_control_data.farVariable) || (new_control_data.nearVariable != _old_control_data.nearVariable) || (new_control_data.variableLevel != _old_control_data.variableLevel))
  {
    if(new_control_data.farVariable == 1)
    {
      viscaSerial.set_focus_far_speed(iface, camera, new_control_data.variableLevel);
    }
    else if(new_control_data.nearVariable == 1)
    {
      viscaSerial.set_focus_near_speed(iface, camera, new_control_data.variableLevel);
    }
    else
    {
      viscaSerial.set_focus_stop(iface, camera);
    }
  }

  if(new_control_data.autofocus != _old_control_data.autofocus)
  {
    if(new_control_data.autofocus == 0)
    {
      viscaSerial.set_focus_auto(iface, camera);
    }
    else
    {
      viscaSerial.set_focus_manual(iface, camera);
    }
  }
  _old_control_data = new_control_data;
  return 0;
}

ViscaControlData viscaSendControl;


/*
 * firmware.h
 *
 *  Created on: 03.02.2015
 *      Author: feesma44884
 */

#ifndef FIRMWARE_FIRMWARE_HPP_
#define FIRMWARE_FIRMWARE_HPP_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

#include <Dynamixel.h>
#include <iwdg.h>
#include <wirish.h>
#include "DynamixelDrive.hpp"

#define DXL_BUS_CONFIG  1 //!<Serial1
#define DXL_BAUD_RATE   2 //!<Baud Rate: 115200

namespace definitions {
  const uint8   vcp_max_buffer_size = VCOM_RX_BUFLEN;  //!< Maximum size of the buffer
  const uint8   num_dynamixel_drives= 5;               //!< Number of dynamixel drives
  const uint16  max_velocity = 50;                     //!< Maximum velocity of the drives
  const float   incs_per_speed_step = 7.89f * 0.001f;  //!< Speed of 1 -> 7,78 inc steps per second
}

class Firmware
{
public:

  /**
   * @brief Standard constructor
   */
  Firmware(void);

  /**
   * @brief Initializes the firmware
   */
  void init(void);

  /**
   * @brief Message loop
   */
  void message_loop(void);

  /**
   * @brief Callback for USB interrupt routine
   *
   * @param buffer[in] Data buffer
   * @param buffer_size[in] Size of data buffer
   */
  void usb_interrupt(byte* buffer, const byte buffer_size);
private:

  volatile uint8      _vcp_rx_buffer[definitions::vcp_max_buffer_size];   //!< Buffer for receiving data directly from interrupt routine
  volatile uint8      _vcp_rx_buffer_size;                                //!< Size of data in the buffer

  volatile uint8      _data_buffer[definitions::vcp_max_buffer_size];     //!< Data buffer
  volatile uint8      _data_buffer_size;                                  //!< Size of the data in the data buffer
  volatile bool       _data_buffer_readable;                              //!< Saves true if data buffer is readable
};

extern Firmware         g_firmware;
extern DynamixelDrive   g_dynamixel_list[definitions::num_dynamixel_drives];
extern Dynamixel        g_dynamixel_manager;

#endif /* FIRMWARE_FIRMWARE_HPP_ */

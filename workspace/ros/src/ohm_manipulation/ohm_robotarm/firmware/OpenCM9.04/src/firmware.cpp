#include "firmware.h"

Firmware g_firmware;
DynamixelDrive g_dynamixel_list[definitions::num_dynamixel_drives] = {DynamixelDrive(160, 0, 20),
                                                                      DynamixelDrive(161, 2048, 20),
                                                                      DynamixelDrive(162, 0, 20),
                                                                      DynamixelDrive(163, 2048, 40),
                                                                      DynamixelDrive(164, 2048, 40)};
Dynamixel        g_dynamixel_manager(DXL_BUS_CONFIG);

//DO NOT EDIT!!!
 __attribute__(( constructor )) void premain() {
    init();
}

void global_usb_interrupt(byte* buffer, byte buffer_size) {
  g_firmware.usb_interrupt(buffer, buffer_size);
}

Firmware::Firmware(void) :
_vcp_rx_buffer(),
_vcp_rx_buffer_size(0),
_data_buffer(),
_data_buffer_size(0),
_data_buffer_readable(false)
{
}

void Firmware::init(void) {
  //disable interrupts
  noInterrupts();

  //set board LED pin as outpu
  pinMode(BOARD_LED_PIN, OUTPUT);

  //switch LED on/off to visualize startup
  for(uint8 n = 0; n < 10; n++) {
    toggleLED();
    delay(100);
  }

  //init dynamixel bus
  g_dynamixel_manager.begin(DXL_BAUD_RATE);
  ;
  //config all dynamixel
  for(uint8 j = 0; j < 2; j++) {
    for(uint8 n = 0; n < definitions::num_dynamixel_drives; n++) {
      //lock eeprom
      g_dynamixel_manager.writeByte(g_dynamixel_list[n]._ID, 0x2F, 1);
      delay(10);

      //write speed
      g_dynamixel_manager.writeWord(g_dynamixel_list[n]._ID, 0x20, g_dynamixel_list[n]._moving_speed);
      delay(10);

      //move to start position
      g_dynamixel_manager.writeWord(g_dynamixel_list[n]._ID, 0x1E, g_dynamixel_list[n]._start_position);

    }
    delay(200);
  }


  //enable interrupts
  interrupts();

  //attach interrupt
  SerialUSB.attachInterrupt(global_usb_interrupt);
}

void Firmware::message_loop(void) {
  static bool dumpMessage = true;
  static const uint8 feedback_ok[3] = {0x01, 0x0D, 0x0A};
  static const uint8 feedback_error[3] = {0x00, 0x0D, 0x0A};

  //check if there is data to process
  if(_data_buffer_readable) {

    //the message after the first run can be ignored
    //check if message can be ignored
    if(dumpMessage) {
      dumpMessage = false;
      _data_buffer_readable = false;
      return;
    }

    //search for ID of the drive
    DynamixelDrive* drive = NULL;
    for(uint8 n = 0; n < definitions::num_dynamixel_drives; n++) {
      if(g_dynamixel_list[n]._ID == (uint8)(_data_buffer[0])) {
        //save ptr
        drive = &g_dynamixel_list[n];
      }
    }

    //check if drive was found
    if(drive == NULL) {
      //send error
      SerialUSB.write((const void*)feedback_error, 3);
    }
    else {
      //process command
      switch(_data_buffer[1]) {
      //target position
      case 0x1E:
      {
        //save target position
        drive->_target_position = (int16)(_data_buffer[2] << 8 | _data_buffer[3]);

        //send feedback
        SerialUSB.write((const void*)feedback_ok, 3);
      }break;
      //actual position
      case 0x24:
      {
        //send actual position
        const uint8 tx_buffer[5]  = {drive->_ID, (uint8)(drive->_actual_position>>8),
                                     (uint8)(drive->_actual_position), '\r', '\n'};

        SerialUSB.write((const void*)(tx_buffer), 5);
      }break;
      }
    }

    //switch off LED
    digitalWrite(BOARD_LED_PIN, HIGH);

    _data_buffer_readable = false;
  }

  //update positions of the drive
  static uint8 dxl_cnt_1  = 0;
  static int time_stamp_commands = 0;
  static int time_stamp_update = 12;

  //check if enough time has passed
  if(((int)(millis()) - time_stamp_commands) > 100) {
    //get ptr to dynamixel drive
    DynamixelDrive* drive = &g_dynamixel_list[dxl_cnt_1++];

    //check if new target position was received
    if(drive->_target_position != drive->_target_position_old) {
      //check distance
      int16 m_position = (int16)((float)(drive->_target_position_old + drive->_actual_position) * 0.5f);
     // float p = (float)(abs(drive->_target_position - m_position)) * 0.01f; // Stand Martin
      float p = (float)(abs(drive->_target_position - m_position)) * 0.1f;

      //maximum value of p
      if(p > 1.0f) p = 1.0f;

      //calculate speed
      drive->_moving_speed = (uint16)((float)(definitions::max_velocity) * p);

      //minimum value for speed
      if(drive->_moving_speed < 5) drive->_moving_speed = 5;

      //send speed
      g_dynamixel_manager.writeWord(drive->_ID, 0x20, (word)(drive->_moving_speed));

      //send position
      g_dynamixel_manager.writeWord(drive->_ID, 0x1E, (word)(drive->_target_position));
      drive->_target_position_old = drive->_target_position;
    }


    //check counter
    if(dxl_cnt_1 >= definitions::num_dynamixel_drives) {
      dxl_cnt_1 = 0;
    }

    //update time
    time_stamp_commands = (int)(millis());
  }

  const int delta_time = (int)(millis()) - time_stamp_update;
  if(delta_time > 25) {
    //update position of all drives -> simulate (get position is not very stable!)
    for(uint8 n = 0; n < definitions::num_dynamixel_drives; n++) {
      DynamixelDrive* drive = &g_dynamixel_list[n];

      //check if drive has to move
      const int16 delta_position = drive->_target_position - drive->_actual_position;
      if(delta_position > 5) {
        //move drive
        const float fposition = definitions::incs_per_speed_step * (float)(drive->_moving_speed) * (float)(delta_time);

        //update position
        drive->_actual_position += (uint16)(fposition);
      }
      else if(delta_position < 5) {
        //move drive
        const float fposition = definitions::incs_per_speed_step * (float)(drive->_moving_speed) * (float)(delta_time);

        //update position
        drive->_actual_position -= (uint16)(fposition);
      }
      else {
        drive->_actual_position = drive->_target_position;
      }

      //check range
      if(drive->_actual_position > (drive->_start_position + 2048)) {
        drive->_actual_position = drive->_start_position + 2048;
      }
      else if(drive->_actual_position < (drive->_start_position - 2048)) {
        drive->_actual_position = drive->_start_position - 2048;
      }

    }

    //update time
    time_stamp_update = millis();
  }
}

void Firmware::usb_interrupt(byte* buffer, const byte buffer_size) {
  //disable interrupts
  noInterrupts();

  //check if receive buffer for overflow
  if((_vcp_rx_buffer_size + buffer_size) >= definitions::vcp_max_buffer_size) {
      //stop receiving and throw data away
    _vcp_rx_buffer_size = 0;
  }
  else {
    //append data to buffer
    for(uint8 i = 0; i < buffer_size; i++) {
      _vcp_rx_buffer[_vcp_rx_buffer_size++] = buffer[i];

      //check for escape sequence
      if(buffer[i] == '\n' && _vcp_rx_buffer_size >= 2) {
        if(_vcp_rx_buffer[_vcp_rx_buffer_size - 2] == '\r') {
          //escape sequence

          //copy data to data buffer
          //check if data buffer is free
          if(_data_buffer_readable == false) {
            _data_buffer_size = 0;
            for(uint8 n = 0; n < _vcp_rx_buffer_size - 2; n++) {
              _data_buffer[_data_buffer_size++] = _vcp_rx_buffer[n];
            }
            _data_buffer_readable = true;

            //swich LED on
            digitalWrite(BOARD_LED_PIN, LOW);
          }

          //rx buffer is cleared
          _vcp_rx_buffer_size = 0;
        }
      }
    }

  }

  //enable interrupts
  interrupts();
}

int main(void) {

  //init firmware
  g_firmware.init();

  //wait
  while(true) {
    g_firmware.message_loop();
  }

  return 0;
}


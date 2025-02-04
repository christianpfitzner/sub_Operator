#include <SPI.h>
#include "mcp_can.h"
#include "mcp_can_dfs.h"

/**
 * @brief Definitions for CAN Interface
 */
 #define CAN_BAUD_RATE      CAN_1000KBPS
 #define CAN_LED_1          7
 #define CAN_LED_2          8
 #define CAN_INTERRUPT      2
 #define CAN_CS             10
 #define CAN_BUFFER_SIZE    8

 /**
  * @brief Virtual Com Port Definitions
  */
  #define VCP_BAUD_RATE           115200
  #define VCP_MAX_RX_BUFFER_SIZE  13 
  #define VCP_MAX_TX_BUFFER_SIZE  2

  /**
   * @namespace global
   * 
   * @brief Global Variables
   */
  namespace global {
    MCP_CAN   can_interface(CAN_CS);                   /***< CAN-Interface */
    
    uint8_t   vcp_tx_buffer[VCP_MAX_TX_BUFFER_SIZE];   /***< Buffer for sending Data via VCP to host*/
    uint8_t   vcp_tx_buffer_size = 0;                  /***< Size of TX buffer*/

    uint8_t   vcp_rx_buffer[VCP_MAX_RX_BUFFER_SIZE];   /***< Buffer for receiving Data via VCP from host*/
    uint8_t   vcp_rx_buffer_size = 0;                  /***< Size of RX buffer*/
    bool      vcp_rx_available;                        /***< Data is received and can be processed */

    uint8_t   can_tx_buffer[CAN_BUFFER_SIZE];          /***< CAN TX buffer */
    uint8_t   can_rx_buffer[CAN_BUFFER_SIZE];          /***< CAN RX buffer */

    bool      clean_up_done = false;                   /***< Saves true if the first clean up was done */
   };

/**
 * @brief Initializes the serial-can converter board
 */
void setup() {

  //configure pin modes
  pinMode(CAN_LED_1, OUTPUT);
  pinMode(CAN_LED_2, OUTPUT);
  pinMode(CAN_INTERRUPT, INPUT);

  //LED-info: init board
  digitalWrite(CAN_LED_1, HIGH);
  digitalWrite(CAN_LED_2, HIGH);

  //wait for serial connection
  while(!Serial);

  //init serial 
  Serial.begin(VCP_BAUD_RATE);

  //init CAN-Interface
  if(global::can_interface.begin(CAN_BAUD_RATE) == CAN_FAILINIT) {
    Serial.println("Failed to init CAN-interface!");
    Serial.println("Waiting for reset...");
    while(1);
  }

  //reset all buffers
  for(uint8_t n = 0; n < VCP_MAX_TX_BUFFER_SIZE; n++) {
    global::vcp_tx_buffer[n] = 0;
  }

  for(uint8_t n = 0; n < VCP_MAX_RX_BUFFER_SIZE; n++) {
    global::vcp_rx_buffer[n] = 0;
  }

  for(uint8_t n = 0; n < CAN_BUFFER_SIZE; n++) {
    global::can_tx_buffer[n] = 0;
    global::can_rx_buffer[n] = 0;
  }

  //reset variables
  global::clean_up_done = false;
  global::vcp_tx_buffer_size = 0;
  global::vcp_rx_buffer_size = 0;
  global::vcp_rx_available = false;

  //LED-info: serial initialized
  digitalWrite(CAN_LED_1, LOW);
  delay(600);
  digitalWrite(CAN_LED_1, LOW);
}

/**
 * @brief Receives the data from the host pc
 */
void vcp_receive_data() {
  //check if serial data is available
  while(Serial.available() > 0) {
    //read character
    uint8_t c = (uint8_t)Serial.read();

    //check if message overflows
    if(global::vcp_rx_buffer_size >= VCP_MAX_RX_BUFFER_SIZE) {
      //delete message
      global::vcp_rx_buffer_size = 0;
      return;
    }

    //check for end of the message
    //the end of the message is recognized by escape sequence '\r' + '\n'
    if(c == '\n') {
      if(global::vcp_rx_buffer[global::vcp_rx_buffer_size - 1] == '\r') {
        global::vcp_rx_available = true;
        global::vcp_rx_buffer_size = global::vcp_rx_buffer_size - 1;
        return;
      }
    }

    //save data to rx buffer
    global::vcp_rx_buffer[global::vcp_rx_buffer_size++] = c;
  }
}

void can_send_data() {
  //write ID
  const uint16_t can_ID = (uint16_t)(global::vcp_rx_buffer[1] << 8 | global::vcp_rx_buffer[2]);

  //calculate CAN data size
  const uint8_t can_data_size = global::vcp_rx_buffer_size - 3;
  
  //write data
  for(uint8_t n = 0; n < CAN_BUFFER_SIZE; n++) {
    if(n < can_data_size) {
      global::can_tx_buffer[n] = global::vcp_rx_buffer[n + 3];
    }
    else {
      global::can_tx_buffer[n] = 0x00;
    }
  }

  //send data via CAN
  global::can_interface.sendMsgBuf(can_ID, 0, CAN_BUFFER_SIZE, global::can_tx_buffer);
}

void can_receive_data() {
  //check if there is data in the CAN buffer
  if(!digitalRead(CAN_INTERRUPT)) {
    //read buffer
    uint8_t can_rx_size = 0;
    global::can_interface.readMsgBuf(&can_rx_size, (byte*)global::can_rx_buffer);

    //read ID
    uint16_t  can_ID = global::can_interface.getCanId();
    
    //generate message
    //write CAN ID
    global::vcp_tx_buffer[0] = (uint8_t)(can_ID >> 8);
    global::vcp_tx_buffer[1] = (uint8_t)(can_ID);
    global::vcp_tx_buffer_size = 2;

    //send data
    //CAN id
    Serial.print((char)(can_ID >> 8));
    Serial.print((char)(can_ID));

    //CAN data
    for(uint8_t n = 0; n < can_rx_size; n++) {
      Serial.print((char)(global::can_rx_buffer[n]));
    }

    //escape sequence
    Serial.println();
  }
  else {
    //no data
    Serial.print((char)(0xff));
    Serial.println();
  }
}

/**
 * @brief Main message loop of the firmware
 */
void loop() {
  //show that there is CAN data with CAN_LED_2
  digitalWrite(CAN_LED_2, !digitalRead(CAN_INTERRUPT));

  //receive data from host
  vcp_receive_data();

  //check if this was the first call
  //if yes flush data 
  if(global::vcp_rx_available == true && global::clean_up_done == false) {
    digitalWrite(CAN_LED_1, HIGH);
    global::vcp_rx_buffer_size = 0;
    global::vcp_rx_available = false;
    global::clean_up_done = true;
  }

  //process the received data
  if(global::vcp_rx_available == true) {
    //check command
    uint8_t cmd = global::vcp_rx_buffer[0];

    switch(cmd) {
      case 0x11: {
        can_send_data();
      }break;
      case 0x44: {
        can_receive_data();
      }
    }

    //message processed
    global::vcp_rx_buffer_size = 0;
    global::vcp_rx_available = false;
  }
  
}


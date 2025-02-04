/*
 * serial_can_definitions.h
 *
 *  Created on: 23.02.2016
 *      Author: feesma44884
 */

#ifndef SERIAL_CAN_DEFINITIONS_H_
#define SERIAL_CAN_DEFINITIONS_H_

/**
 * @brief Definitions
 */
#define RS232_SERIAL_ASYNC_MAX_BUFFER_SIZE 32

/**
 * Macro to safe delete allocated memory
 */
#define SAFE_DELETE(x)  {if(x != NULL) {delete(x); x = NULL;}}



#endif /* SERIAL_CAN_DEFINITIONS_H_ */

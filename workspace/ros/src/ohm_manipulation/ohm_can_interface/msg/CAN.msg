#CAN.msg
#author: Martin Fees
#This message is used to transmit and receive CAN messages
Header header
uint16 ID		#CAN identifier
uint8[8] data	#CAN 8 byte of data
uint8 data_size	#Data size used in canData (<= 8)
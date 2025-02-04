/*
 * CRC_8.cpp
 *
 *  Created on: 08.12.2014
 *      Author: m1ch1
 */

#include "CRC_8.h"
namespace {
const uint8_t CRC8MASK = 0xA6;
}


namespace apps
{



CRC_8::CRC_8()
{

}

CRC_8::~CRC_8()
{

}

unsigned int CRC_8::computeCRC(uint8_t* data, unsigned int length)
{
   unsigned int num_databits = length * 8;
   uint8_t* data_stream = new uint8_t[num_databits];
   this->toBitStream(data_stream, data, length);

   uint8_t crc8 = 0; /* Schieberegister */

   int i;
   for (i = 0; i < num_databits; ++i)
   {
       if (((crc8 & 0x80) ? 1 : 0) != data_stream[i])
          crc8 = (crc8 << 1) ^ CRC8MASK;
       else
          crc8 <<= 1;
   }
   delete[] data_stream;
   return crc8;
}

} /* namespace apps */

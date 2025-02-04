/*
 * CRC_base.h
 *
 *  Created on: 08.12.2014
 *      Author: m1ch1
 */

#ifndef CHECKSUM_CRC_CRC_BASE_H_
#define CHECKSUM_CRC_CRC_BASE_H_

#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


namespace apps
{

class CRC_base
{
public:
   //CRC_base();
   virtual ~CRC_base() { }

   virtual unsigned int computeCRC(uint8_t* data, unsigned int length) = 0;

protected:
   /**
    * @brief Converts a ByteArray into a bit array(as uint8_t)
    *
    * @param data_stream[out] -> Array of bits as uint8_t, length of data_length * 8, must be allocated
    * @param data[in]         -> Array of data
    * @param data_length      -> Arraylength of data
    */
   void toBitStream(uint8_t* data_stream, uint8_t* data , unsigned int data_length)
   {
      for(unsigned int i = 0; i < data_length; ++i)
         for(unsigned int k = 0; k < 8; ++k)
            data_stream[i*8+k] = (data[i] >> (7-k)) & 1;
   }

};

} /* namespace apps */

#endif /* CHECKSUM_CRC_CRC_BASE_H_ */

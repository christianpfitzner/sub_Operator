/*
 * CRC_8.h
 *
 *  Created on: 08.12.2014
 *      Author: m1ch1
 */

#ifndef CHECKSUM_CRC_CRC_8_H_
#define CHECKSUM_CRC_CRC_8_H_

#include "CRC_base.h"

namespace apps
{

class CRC_8: public CRC_base
{
public:
   CRC_8();
   virtual ~CRC_8();

   virtual unsigned int computeCRC(uint8_t* data, unsigned int length);
};

} /* namespace apps */

#endif /* CHECKSUM_CRC_CRC_8_H_ */

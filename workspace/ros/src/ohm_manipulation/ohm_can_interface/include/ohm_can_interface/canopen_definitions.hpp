/*
 * canopen_definitions.hpp
 *
 *  Created on: 18.12.2015
 *      Author: feesma44884
 */

#ifndef CANOPEN_DEFINITIONS_HPP_
#define CANOPEN_DEFINITIONS_HPP_

/**
 * @brief Global Definitions
 */
#define OHM_CANOPEN_COB_SIZE        0x80


/**
 * @namespace CANopen
 *
 * @brief Namespace which define all objects needed to handle CANopen network
 */
namespace CANopen
{

/**
 * @brief Definitions of enumerations
 */

/**
 * @enum COB
 *
 * @brief Enumeration of CANopen Object Types
 */
enum    COB
{
    COB_NMT         =   0x0000,//!< COB_NMT
    COB_SYNC        =   0x0080,//!< COB_SYNC
    COB_TIME_STAMP  =   0x0100,//!< COB_TIME_STAMP
    COB_EMERGENCY   =   0x0081,//!< COB_EMERGENCY
    COB_PDO_T_1     =   0x0181,//!< COB_PDO_T_1
    COB_PDO_R_1     =   0x0201,//!< COB_PDO_R_1
    COB_PDO_T_2     =   0x0281,//!< COB_PDO_T_2
    COB_PDO_R_2     =   0x0301,//!< COB_PDO_R_2
    COB_PDO_T_3     =   0x0381,//!< COB_PDO_T_3
    COB_PDO_R_3     =   0x0401,//!< COB_PDO_R_3
    COB_PDO_T_4     =   0x0481,//!< COB_PDO_T_4
    COB_PDO_R_4     =   0x0501,//!< COB_PDO_R_4
    COB_SDO_T       =   0x0581,//!< COB_SDO_T
    COB_SDO_R       =   0x0601,//!< COB_SDO_R
    COB_ERROR       =   0x0701,//!< COB_ERROR
    COB_END         =   0x077f //!< COB_END
};

/**
 * @brief Definitions of structures
 */
struct MSG;        //!< See msg.hpp

/**
 * @brief Definitions of classes
 */
class OD;                 //!< See OD.hpp
class Manager;            //!< See manager.hpp
class NMT;                //!< See NMT.hpp
class SDO;                //!< See SDO.hpp
class PDO;                //!< See PDO.hpp
class Node;               //!< See node.hpp

/**
 * @brief Definition of functions (look at cpp files)
 */
std::shared_ptr<CANopen::Manager> const IManager(void);                //!< See manager.cpp -> Returns signleton of manager class
OD*                               const IOD(void);                     //!< See manager.cpp -> Returns pointer of OD class (saved in Manager)
void     debug_show_message(const char* topic, const MSG& msg);        //!< See manager.cpp -> Prints can messages

}

#endif /* CANOPEN_DEFINITIONS_HPP_ */

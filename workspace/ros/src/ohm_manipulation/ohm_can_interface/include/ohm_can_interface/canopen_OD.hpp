/*
 * canopen_OD.hpp
 *
 *  Created on: 27.01.2016
 *      Author: feesma44884
 */

#ifndef CANOPEN_OD_HPP_
#define CANOPEN_OD_HPP_

/**
 * @class OD
 *
 * @brief Class representing objects from the CANopen Object Dictionary
 *
 * For more information of the object dictionary see DS301 Reference Guide from Elmo Motion Control
 * http://elmomc-germany.de/filefolder/Manuals%20%28CANopen%29/Gold/MAN-G-DS301.pdf
 */
class CANopen::OD
{
public:
  /**
   * @brief Enumeration of objects in object dictionary
   */
  struct Object
  {
    Object(const uint16_t index, const std::string& name, const uint8_t size) :
      _index(index), _name(name), _size(size) {}

    const uint16_t      _index;   //!< Index of the object (see DS301)
    const std::string   _name;    //!< Name of the object (see ObjectName enumeration)
    const uint8_t       _size;    //!< Size of the object in bits
  };
  /**
   * @brief Constructor of class is private
   */
   OD(void);

  /**
   * @brief Destructor of OD class
   */
  ~OD(void);

  /**
   * @return the singleton object of the object dictionary class
   */
  static  OD*        instance(void);

  /**
   * @brief Registers a object to the list
   *
   * @param index[in] Index of the object
   * @param name[in] Name of the object
   * @param size[in]  Size of the object in bits
   */
  bool               add(const uint16_t index, const std::string& name, const uint8_t size);

  /**
   * @param index[in] Index of the object
   * @return reference to object
   */
  const Object&      get(const uint16_t index);

  /**
   * @param name[in] Name of the object
   * @return reference to the object. If object does not exist it returns an object with index and size 0xff
   */
  const Object&      get(const std::string& name);

private:

    std::map<uint16_t, Object>        _object_dictionary;     //!< Map for saving the object dictionary sorted by index
};

#endif /* CANOPEN_OD_HPP_ */

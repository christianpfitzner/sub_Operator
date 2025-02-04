/*
 * singleton.hpp
 *
 *  Created on: 09.03.2016
 *      Author: feesma44884
 */

#ifndef SINGLETON_H_
#define SINGLETON_H_

#include <memory>

/***
 * @namespace ohm_robotarm
 *
 * @brief Namespace of the AutonOHM robotarm
 */
namespace CANopen
{
  /**
   * @class Singleton
   *
   * @brief Singleton parent class
   */
  template <typename T>
  class Singleton
  {
  public:
    /**
     * @brief Creates a single instance of the class T
     *
     * @return instance of the class
     */
     static std::shared_ptr<T> instance ()
     {
        if (!_instance)
           _instance = std::shared_ptr<T>(new T ());
        return _instance;
     }

     /**
      * @brief Destructor
      */
     virtual ~Singleton ()
     {
       release();
     }

     /**
      * @brief Releases the allocated memory
      */
     static void release(void) {
       if(_instance) {
         _instance.reset();
        }
     }
  protected:
     static std::shared_ptr<T> _instance; //!< Static instance of the singleton class

     /**
      * @brief Constructor
      */
     Singleton () { }
  };

  template <typename T> std::shared_ptr<T> Singleton <T>::_instance = nullptr;
};

#endif /* SINGLETON_H_ */

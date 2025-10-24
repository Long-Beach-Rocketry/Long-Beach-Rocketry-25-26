/**
 * @brief This file sets up the generic USART driver for sending and receiving information.
 * 
 */

#pragma once
#include <span>
#include <string>

/**
  * @brief class that contains the functions needed to intialize and setup rx and tx functions.
  */
class Usart
{
public:
    /**
      * @brief Sends data through serial tx data line.
      * 
      * @param data Data to be sent passed as a string variable.
      * @param size Size of data variable containing message;
      * 
      */
    virtual void send_tx(const uint8_t* data, size_t size) = 0;

    /**
      * @brief Recieves data from serial rx data line input.
      * 
      * @param data A uint8_t pointer used to store recieved serial input.
      * 
      * @return True on success.
      */
    virtual bool receive_rx(uint8_t* data, size_t size) = 0;
};
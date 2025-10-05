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
      * 
      */
    virtual void send_tx(const std::span<char> data) = 0;

    /**
      * @brief Recieves data from serial rx data line input.
      * 
      * @param data A variable of type string to store recieved serial input.
      * 
      * @return True on success.
      */
    virtual bool receive_rx(std::span<char>& data) = 0;
};
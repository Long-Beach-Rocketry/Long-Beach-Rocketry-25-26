/**
 * @brief This file sets up the generic USART driver for sending and receiving information.
 * 
 */

#pragma once
#include <cstdint>
#include <span>

/**
  * @brief class that contains the functions needed to intialize and setup rx and tx functions.
  */
class Usart
{
public:
    /**
      * @brief Sends data through serial tx data line.
      * 
      * @param txbuf Data to be sent as a uint8_t std::array
      * @return True on success. False, otherwise.
      * 
      */
    virtual bool send_tx(std::span<const uint8_t> txbuf) = 0;

    /**
      * @brief Recieves data from serial rx data line input.
      * 
      * @param rxbuf A uint8_t std::array to store data being received
      * 
      * @return True on success. False, otherwise.
      */
    virtual bool receive_rx(std::span<uint8_t> rxbuf) = 0;
};
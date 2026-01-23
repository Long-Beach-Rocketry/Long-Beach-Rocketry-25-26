/**
 * @brief This file sets up the generic USART driver for sending and receiving information.
 * 
 */

#pragma once
#include <cstdint>
#include <span>

namespace LBR
{
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
    virtual bool Send(std::span<const uint8_t> txbuf) = 0;

    /**
     * @brief Recieves data from serial rx data line input.
     * 
     * @param byte Reference to store received byte
     * @return True if byte was received, false if no data available
     */
    virtual bool Receive(uint8_t& byte) = 0;
};
}  // namespace LBR
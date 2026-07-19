/**
 * @file usart_andrew.h
 * @brief This file sets up the generic USART driver for sending and receiving information.
 * @author Andrew Pham
 */

#pragma once
#include <cstdint>
#include <span>

namespace LBR
{
/**
  * @brief class that contains the functions needed to initialize and setup rx and tx functions
  * for transmitting and receiving operations.
  */
class UsartAndrew
{
public:
    /**
      * @brief sends data through serial tx data line.
      * 
      * @param tx_buffer Data to be sent as a uint8_t std::array
      * @return True on success. False, otherwise.
      * 
      */
    virtual bool transmit(std::span<const uint8_t> tx_buffer) = 0;

    /**
     * @brief Receives data from serial rx data line input.
     * 
     * @param byte Reference to store received byte
     * @return True if byte was received, false if no data available
     */
    virtual bool receive(uint8_t& byte) = 0;

    /**
    * @brief Virtual destructor for the USART interface.
    */
    virtual ~UsartAndrew() = default;
};
}  // namespace LBR
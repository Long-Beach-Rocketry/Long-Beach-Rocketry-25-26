/**
* @file can.h
* @author Bex Saw
* @brief CAN generic driver interface.
* @date 4/11/2026
*/

#pragma once 

/**
* @note I want to make this CAN driver be able to taking in the CAN2.0 & CAN FD
*       FEATURE: CAN FD support, which means we need to be able to support different data lengths and bit rates.
*       FEATURE: CAN filter configuration, which allows us to filter incoming messages based on their IDs
*       FEATURE: CAN error handling, which allows us to detect and handle errors on the CAN bus.
*       FEATURE: CAN bus timing configuration, which allows us to configure the timing parameters of the
*       CAN bus to ensure reliable communication.
*       FEATURE: CAN message transmission and reception, which allows us to send and receive messages on the CAN bus.
*       FEATURE: CAN bus diagnostics, which allows us to monitor the health of the CAN bus and identify any issues that may arise.
*
*/

namespace LBR 
{
/**
 * @class Can
 * @brief CAN driver instance
 */
class Can
{
public:

 ~Can() = default;

 // virtual functions for CAN 

}; 
} // namespace LBR
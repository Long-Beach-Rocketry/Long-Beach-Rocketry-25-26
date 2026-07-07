/**
 * @file sx1262.h
 * @author Kent Hong
 * @brief SX1262 RF Transceiver Driver Interface
 */
#pragma once

#include <cstdint>
#include <span>
#include "spi.h"
#include "sx1262_enums.h"

namespace LBR
{

struct Sx1262Settings
{
};

struct Sx1262Params
{
    Sx1262Settings settings;
    Spi& spi;
};

class Sx1262
{
public:
    /**
     * @brief Constructor
     * 
     * @param params_ SX1262 settings and spi instance
     */
    explicit Sx1262(Sx1262Params& params_);

    /**
     * @brief Transmit bytes from a buffer with specified values.
     * 
     * @return true transmit success, false otherwise
     */
    bool transmit(const std::span<const uint8_t>& tx_buf);

    /**
     * @brief Receive bytes and store in buffer
     * 
     * @param buf 
     * @return true Bytes received and stored in buffer successfully, false otherwise
     */
    bool receive(const std::span<uint8_t>& rx_buf);

    /**
     * @brief Change settings within the SX1262 during runtime
     * 
     * @param new_settings 
     * @return true Settings changed successfully, false otherwise
     */
    bool set_settings(Sx1262Settings new_settings);

private:
    Sx1262Settings settings;
    Spi& spi;
};
}  // namespace LBR

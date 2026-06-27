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
    bool transmit(std::span<const uint8_t>& buf);

    /**
     * @brief Receive bytes and store in buffer
     * 
     * @param buf 
     * @return true 
     * @return false 
     */
    bool receive(std::span<uint8_t>& buf);
    bool set_settings(Sx1262Settings new_settings);

private:
};
}  // namespace LBR

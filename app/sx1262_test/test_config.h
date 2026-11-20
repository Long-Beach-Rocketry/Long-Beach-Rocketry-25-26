/**
 * @file test_config.h
 * @author Alex Pulido
 * @brief Configuration for SX1262 test payload and settings.
 */
#pragma once

#include <array>
#include <cstdint>

#include "sx1262.h"

namespace LBR
{

inline constexpr std::array<uint8_t, 14> kSx1262TestPayload{
    'I', 'M', ' ', 'A', ' ', 'C', 'A', 'T', ' ', 'M', 'E', 'O', 'W', '!'};

inline Sx1262Settings sx1262_test_settings()
{
    // Assumes a bare SX1262 with a crystal, LDO regulator, and no
    // DIO2-controlled RF switch. Set use_tcxo, use_dcdc, and
    // dio2_as_rf_switch for the installed module on both firmware images.
    Sx1262Settings settings;
    settings.frequency_hz = 915'000'000;
    settings.spreading_factor = 7;
    settings.bandwidth = LoRaBandwidth::KHZ_125;
    settings.coding_rate = CodingRate::CR_4_5;
    settings.tx_power_dbm = 10;
    settings.rx_timeout = 0x00FA00;
    return settings;
}

}  // namespace LBR

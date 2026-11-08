/**
 * @file sx1262_enums.h
 * @author Kent Hong
 * @brief Enums that define settings for the SX1262 RF Trasceiver from the datasheet
 */
#pragma once
#include <stdint.h>

namespace LBR{
    enum class HeaderType : uint8_t{
        EXPLICIT = 0x00,
        IMPLICIT
    };

    enum class CrcType: uint8_t{
        OFF = 0x00,
        ON
    };

    enum class InvertIQ: uint8_t{
        STANDARD = 0x00,
        INVERTED
    };
}
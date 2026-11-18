/**
 * @file sx1262_enums.h
 * @author Kent Hong
 * @brief Enums that define settings for the SX1262 RF Trasceiver from the datasheet
 */
#pragma once
#include <stdint.h>

namespace LBR
{
enum class HeaderType : uint8_t
{
    EXPLICIT = 0x00,
    IMPLICIT
};

enum class CrcType : uint8_t
{
    OFF = 0x00,
    ON
};

enum class InvertIQ : uint8_t
{
    STANDARD = 0x00,
    INVERTED
};

enum class StdbyConfig : uint8_t
{
    RC = 0x00,
    XOSC
};

// LoRa bandwidth codes from Table 6-2 of the SX1262 datasheet.
enum class LoRaBandwidth : uint8_t
{
    KHZ_7 = 0x00,
    KHZ_10 = 0x01,
    KHZ_15 = 0x02,
    KHZ_20 = 0x03,
    KHZ_31 = 0x04,
    KHZ_41 = 0x05,
    KHZ_62 = 0x06,
    KHZ_125 = 0x07,
    KHZ_250 = 0x08,
    KHZ_500 = 0x09,
};

enum class CodingRate : uint8_t
{
    CR_4_5 = 0x01,
    CR_4_6 = 0x02,
    CR_4_7 = 0x03,
    CR_4_8 = 0x04,
};

// DIO3 voltage while it is used as the TCXO control output.
enum class TcxoVoltage : uint8_t
{
    V_1_6 = 0x00,
    V_1_7 = 0x01,
    V_1_8 = 0x02,
    V_2_2 = 0x03,
    V_2_4 = 0x04,
    V_2_7 = 0x05,
    V_3_0 = 0x06,
    V_3_3 = 0x07,
};

// Command opcodes from the SX1262 command tables in Section 11
enum class Opcode : uint8_t
{
    SET_STANDBY = 0x80,
    SET_RX = 0x82,
    SET_TX = 0x83,
    STOP_TIMER_ON_PREAMBLE = 0x9F,
    SET_REGULATOR_MODE = 0x96,
    CALIBRATE = 0x89,
    CALIBRATE_IMAGE = 0x98,
    SET_PA_CONFIG = 0x95,
    SET_RX_TX_FALLBACK_MODE = 0x93,
    WRITE_REGISTER = 0x0D,
    READ_REGISTER = 0x1D,
    WRITE_BUFFER = 0x0E,
    READ_BUFFER = 0x1E,
    SET_DIO_IRQ_PARAMS = 0x08,
    GET_IRQ_STATUS = 0x12,
    CLR_IRQ_STATUS = 0x02,
    SET_DIO2_AS_RF_SWITCH = 0x9D,
    SET_DIO3_AS_TCXO_CTRL = 0x97,
    SET_RF_FREQUENCY = 0x86,
    SET_PACKET_TYPE = 0x8A,
    SET_TX_PARAMS = 0x8E,
    SET_MODULATION_PARAMS = 0x8B,
    SET_PACKET_PARAMS = 0x8C,
    SET_BUFFER_BASE_ADDRESS = 0x8F,
    GET_STATUS = 0xC0,
    GET_RX_BUFFER_STATUS = 0x13,
};

}  // namespace LBR
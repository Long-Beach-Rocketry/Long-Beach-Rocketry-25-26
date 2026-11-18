/**
 * @file sx1262.h
 * @author Kent Hong
 * @brief SX1262 RF Transceiver Driver Interface
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include "gpio_cs.h"
#include "spi.h"
#include "sx1262_enums.h"

namespace LBR
{

/**
 * @brief LoRa link settings programmed by init() and set_settings().
 *
 * Both radios must use the same frequency, spreading factor, bandwidth,
 * coding rate, sync word, header type, CRC, and IQ polarity.
 */
struct Sx1262Settings
{
    // US 902-928 MHz. 915 MHz is an exact PLL step (0x39300000).
    uint32_t frequency_hz = 915'000'000;

    // 5..12. SF9 is the rocket default: longer range than SF7, still a
    // short packet. SF11/SF12 at 125 kHz need low-data-rate optimize.
    uint8_t spreading_factor = 9;

    // Datasheet LoRa BW code. 125 kHz is KHZ_125 (0x07), not 0x04.
    LoRaBandwidth bandwidth = LoRaBandwidth::KHZ_125;

    // 4/5. Raise this only if the link is marginal.
    CodingRate coding_rate = CodingRate::CR_4_5;

    // Explicit override. LDRO is also enabled automatically when symbol time
    // is equal to or greater than 16.38 ms by ldro_required().
    bool low_data_rate_optimize = false;

    // Datasheet default. 8 is the practical minimum. Do not use 65535.
    uint16_t preamble_symbols = 12;

    HeaderType header = HeaderType::EXPLICIT;
    CrcType crc = CrcType::ON;
    InvertIQ iq = InvertIQ::STANDARD;

    // Private network. 0x3444 is the public LoRaWAN sync word.
    uint16_t sync_word = 0x1424;

    // SX1262 maximum. Requires the PA pair below and ocp_trim of 0x38.
    int8_t tx_power_dbm = 22;
    uint8_t pa_duty_cycle = 0x04;
    uint8_t hp_max = 0x07;

    // 200 us ramp (datasheet ramp-time code 0x04).
    uint8_t ramp_time = 0x04;

    // SetTx timeout in 15.625 us steps. 0 disables the chip timeout.
    uint32_t tx_timeout = 0;

    // SetRx timeout in 15.625 us steps. 0x00FA00 is 1 second, single RX.
    // 0x000000 never times out. 0xFFFFFF is continuous RX, not a timeout.
    uint32_t rx_timeout = 0x00FA00;

    // Over-current limit. 0x38 is about 140 mA, the SX1262 default.
    uint8_t ocp_trim = 0x38;

    // True only if the module has the DC-DC inductor. Otherwise stay on
    // the LDO, or the radio will not run.
    bool use_dcdc = false;

    // True only if a TCXO is wired to XTA and controlled by DIO3. Leaving
    // this false on a TCXO module holds BUSY high and init() fails.
    bool use_tcxo = false;
    TcxoVoltage tcxo_voltage = TcxoVoltage::V_3_3;

    // TCXO start timeout in 15.625 us steps. 320 is 5 ms.
    uint32_t tcxo_timeout = 320;

    // True when the module uses DIO2 to enable the TX RF switch. Most
    // E22-style modules need this, or the PA never reaches the antenna.
    bool dio2_as_rf_switch = false;
};

struct Sx1262Params
{
    Sx1262Settings settings;
    Spi& spi;
    GpioChipSelect& cs;
    Gpio& reset;
    Gpio& busy;
};

class Sx1262
{
public:
    /**
     * @brief Constructor.
     *
     * SPI must be mode 0 (CPOL 0, CPHA 0), MSB first. NSS is active low
     * through GpioChipSelect. reset is active-low NRESET. busy is the
     * SX1262 BUSY pin, high while a command is being processed.
     *
     * The SPI and GPIO objects must outlive this driver.
     *
     * @param params_ Settings, SPI, chip select, reset, and busy.
     */
    explicit Sx1262(Sx1262Params& params_);

    /**
     * @brief Reset the transceiver and program the LoRa settings.
     *
     * @return true if the chip answers in STDBY_RC, false otherwise.
     */
    bool init();

    /**
     * @brief Transmit one LoRa packet.
     *
     * Frequency, modulation, PA, and sync word come from init().
     *
     * @return true if TxDone fired, false on timeout or a command error.
     */
    bool transmit(const std::span<const uint8_t>& tx_buf);

    /**
     * @brief Receive one LoRa packet into rx_buf.
     *
     * @param rx_buf Buffer large enough for the expected payload.
     * @return The payload length in bytes if the packet is received;
     *         otherwise, 0 on timeout, CRC/header error, or if the packet
     *         does not fit in rx_buf.
     */
    std::size_t receive(const std::span<uint8_t>& rx_buf);

    /**
     * @brief Store new settings and, after init(), program them.
     *
     * @param new_settings Replacement link settings.
     * @return true if the settings are valid and were applied.
     */
    bool set_settings(Sx1262Settings new_settings);

private:
    bool wait_busy();
    bool write_command(std::span<uint8_t> command);
    bool read_command(uint8_t command, std::span<uint8_t> response);
    bool write_register(uint16_t address, std::span<const uint8_t> data);
    bool read_register(uint16_t address, uint8_t& value);
    bool set_standby(StdbyConfig mode);
    bool set_packet_params(uint8_t payload_length);
    bool set_dio_irq(uint16_t mask);
    bool fix_iq_polarity();
    bool fix_modulation_quality();
    bool clear_irq(uint16_t mask);
    bool get_irq(uint16_t& irq);
    bool in_standby_rc();
    bool configure();

    Sx1262Settings settings;
    Spi& spi;
    GpioChipSelect& cs;
    Gpio& reset_pin;
    Gpio& busy_pin;
    bool initialized;
};

}  // namespace LBR

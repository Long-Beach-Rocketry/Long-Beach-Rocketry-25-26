/**
 * @file sx1262.cc
 * @brief SX1262 LoRa configuration and blocking packet operations.
 *
 * The driver communicates over SPI with a separate active-low chip select,
 * reset, and BUSY pin. Initialize the radio before transmitting or receiving.
 * The reset delays and polling timeouts require a running millisecond tick.
 */

#include "sx1262.h"
#include <array>
#include <cstdint>
#include <span>

#include "delay.h"

/**
 * @brief Anonymous namespace for internal constants and helper functions.
 */
namespace
{
constexpr uint32_t kXtalHz = 32'000'000;
constexpr uint32_t kPllScalingFactor = 1u << 25;
constexpr uint16_t kRegSyncWord = 0x0740;
constexpr uint16_t kRegIqPolarity = 0x0736;
constexpr uint16_t kRegTxModulation = 0x0889;
constexpr uint16_t kRegOcp = 0x08E7;
constexpr uint8_t kLoRaPacketType = 0x01;
constexpr uint8_t kPaLut = 0x01;
constexpr uint8_t kSx1262DeviceSel = 0x00;
constexpr uint8_t kFallbackStandbyRc = 0x20;
constexpr uint8_t kCalibrateAll = 0x7F;
constexpr uint32_t kBusyTimeoutMs = 2000;
constexpr uint32_t kHostGuardMs = 20000;

// IRQ flags, datasheet Table 13-29.
constexpr uint16_t kIrqTxDone = 1u << 0;
constexpr uint16_t kIrqRxDone = 1u << 1;
constexpr uint16_t kIrqHeaderErr = 1u << 5;
constexpr uint16_t kIrqCrcErr = 1u << 6;
constexpr uint16_t kIrqTimeout = 1u << 9;
constexpr uint16_t kIrqTxMask = kIrqTxDone | kIrqTimeout;
constexpr uint16_t kIrqRxMask =
    kIrqRxDone | kIrqHeaderErr | kIrqCrcErr | kIrqTimeout;

// LoRa bandwidth table (Table 6-2), in Hz, indexed by the datasheet BW code.
constexpr uint32_t kBandwidthHz[] = {
    7810, 10420, 15630, 20830, 31250, 41670, 62500, 125000, 250000, 500000,
};

/**
 * @brief Convert an opcode enum to the byte sent over SPI.
 * @param command SX1262 command opcode.
 * @return Numeric opcode.
 */
constexpr uint8_t command_byte(LBR::Opcode command)
{
    return static_cast<uint8_t>(command);
}

/**
 * @brief Convert an RF frequency in hertz to an SX1262 PLL tuning word.
 *
 * The SetRfFrequency command does not accept a frequency directly in hertz.
 * Instead, it requires a 32-bit synthesizer tuning word (RF_Freq) that is
 * converted internally by the SX1262 PLL.
 *
 * SX1262 Datasheet:
 * - Section 13.4.1 SetRfFrequency
 * - Table 13-36
 *
 * @code
 * RF_frequency = (RF_Freq * F_XTAL) / 2^25
 * @endcode
 *
 * Rearranging to solve for RF_Freq:
 *
 * @code
 * RF_Freq = (RF_frequency * 2^25) / F_XTAL
 * @endcode
 *
 * where:
 * - RF_frequency is the desired RF frequency in hertz.
 * - F_XTAL is the SX1262 reference crystal frequency (32 MHz).
 * - RF_Freq is the 32-bit PLL tuning word sent to SetRfFrequency.
 *
 * Example for 915 MHz:
 *
 * @code
 * RF_Freq =
 *     (915'000'000 * 2^25) / 32'000'000
 *   = 959'447'040
 *   = 0x39300000
 * @endcode
 *
 * The returned value becomes bytes 1-4 of the SetRfFrequency
 * command payload.
 *
 * @param frequency_hz Desired RF frequency in hertz.
 * @return 32-bit PLL tuning word for SetRfFrequency.
 */
constexpr uint32_t frequency_to_pll_word(uint32_t frequency_hz)
{
    return static_cast<uint32_t>(
        (static_cast<uint64_t>(frequency_hz) * kPllScalingFactor) / kXtalHz);
}

// Verify that the PLL conversion produces the expected value for 915 MHz.
static_assert(frequency_to_pll_word(915'000'000) == 0x39300000u);

/**
 * @brief Check the setting ranges used by this driver.
 *
 * This validates individual fields, not board compatibility or every
 * combination of settings.
 *
 * @param value Settings to validate.
 * @return true if the checked fields are within their accepted ranges.
 */
bool settings_ok(const LBR::Sx1262Settings& value)
{
    if (value.frequency_hz < 150'000'000u || value.frequency_hz > 960'000'000u)
    {
        return false;
    }
    if (value.spreading_factor < 5 || value.spreading_factor > 12)
    {
        return false;
    }
    if (static_cast<uint8_t>(value.bandwidth) >
        static_cast<uint8_t>(LBR::LoRaBandwidth::KHZ_500))
    {
        return false;
    }
    const auto coding_rate = static_cast<uint8_t>(value.coding_rate);
    if (coding_rate < static_cast<uint8_t>(LBR::CodingRate::CR_4_5) ||
        coding_rate > static_cast<uint8_t>(LBR::CodingRate::CR_4_8))
    {
        return false;
    }
    if (value.preamble_symbols == 0)
    {
        return false;
    }
    if (value.tx_power_dbm < -9 || value.tx_power_dbm > 22)
    {
        return false;
    }
    if (value.pa_duty_cycle > 0x07 || value.hp_max > 0x07 ||
        value.ramp_time > 0x07)
    {
        return false;
    }
    if (value.tx_timeout > 0x00FFFFFFu || value.rx_timeout > 0x00FFFFFFu ||
        value.tcxo_timeout > 0x00FFFFFFu)
    {
        return false;
    }
    if (static_cast<uint8_t>(value.tcxo_voltage) >
        static_cast<uint8_t>(LBR::TcxoVoltage::V_3_3))
    {
        return false;
    }
    return true;
}

/**
 * @brief Determine whether the LoRa symbol time is equal to or greater than 16.38 ms.
 *
 * The SX1262 datasheet recommends LDRO when symbol time is equal to or
 * greater than 16.38 ms (Section 6.1.1.4 and SetModulationParams).
 *
 * @param value Settings containing the spreading factor and bandwidth.
 * @return true if low-data-rate optimization is required by this threshold.
 */
bool ldro_required(const LBR::Sx1262Settings& value)
{
    // The SX1262 supports spreading factors SF5 through SF12.
    if (value.spreading_factor < 5 || value.spreading_factor > 12)
    {
        return false;
    }

    // The enum value is also the datasheet bandwidth-code index in kBandwidthHz.
    const auto bw = static_cast<uint8_t>(value.bandwidth);
    if (bw >= (sizeof(kBandwidthHz) / sizeof(kBandwidthHz[0])))
    {
        return false;
    }
    const uint64_t symbol_num = uint64_t{1} << value.spreading_factor;

    // Cross-multiply symbol_num / bandwidth >= 1638 / 100000
    // to compare symbol time against 16.38 ms without floating-point math.
    return symbol_num * 100000u >= uint64_t{kBandwidthHz[bw]} * 1638u;
}

/** @brief Frequency-band parameters passed to CalibrateImage. */
struct ImageCal
{
    uint8_t freq1;  ///< Lower calibration frequency parameter.
    uint8_t freq2;  ///< Upper calibration frequency parameter.
};

/**
 * @brief Select image-calibration parameters for an RF frequency.
 *
 * SX1262 Datasheet:
 * - Section 9.6 Image Rejection Calibration
 * - Table 9-2 Image Calibration Frequency Ranges
 *
 * Uses the table's values for supported bands; otherwise, constructs a
 * narrow frequency window using the command's 4 MHz units.
 *
 * @param frequency_hz Desired RF frequency in hertz.
 * @return Two parameters for the CalibrateImage command.
 */
ImageCal image_cal_for(uint32_t frequency_hz)
{
    if (frequency_hz >= 902'000'000u && frequency_hz <= 928'000'000u)
    {
        return {0xE1, 0xE9};
    }
    if (frequency_hz >= 863'000'000u && frequency_hz <= 870'000'000u)
    {
        return {0xD7, 0xDB};
    }
    if (frequency_hz >= 779'000'000u && frequency_hz <= 787'000'000u)
    {
        return {0xC1, 0xC5};
    }
    if (frequency_hz >= 470'000'000u && frequency_hz <= 510'000'000u)
    {
        return {0x75, 0x81};
    }
    if (frequency_hz >= 430'000'000u && frequency_hz <= 440'000'000u)
    {
        return {0x6B, 0x6F};
    }

    // Outside the tabulated ISM bands, calibrate a narrow window around
    // the selected frequency. The bytes are MHz / 4.
    const uint32_t mhz = frequency_hz / 1'000'000u;
    const auto freq1 = static_cast<uint8_t>(mhz / 4u);
    auto freq2 = static_cast<uint8_t>((mhz + 4u) / 4u);
    if (freq2 <= freq1)
    {
        freq2 = static_cast<uint8_t>(freq1 + 1u);
    }

    return {freq1, freq2};
}
}  // namespace

namespace LBR
{
/**
 * @brief Create a driver using the supplied settings and hardware interfaces.
 * @param params_ Settings and references to SPI, chip select, reset, and BUSY.
 * @note The referenced hardware interfaces must outlive the driver.
 */
Sx1262::Sx1262(Sx1262Params& params_)
    : settings{params_.settings},
      spi{params_.spi},
      cs{params_.cs},
      reset_pin{params_.reset},
      busy_pin{params_.busy},
      initialized{false}
{
}

/**
 * @brief Reset the radio, apply the settings, and check its standby status.
 * @return true if initialization completes and the radio reports STDBY_RC;
 *         false if settings, reset, communication, or the status check fails.
 * @note Requires a running millisecond tick for reset delays and BUSY polling.
 */
bool Sx1262::init()
{
    initialized = false;
    if (!settings_ok(settings))
    {
        return false;
    }

    // NSS must be high before reset. NRESET is active low; the datasheet
    // minimum pulse is 100 us. 150 ms matches the previous board timing.
    cs.cs_disable();
    if (!reset_pin.set(false))
    {
        return false;
    }
    LBR::Utils::DelayMs(150);
    if (!reset_pin.set(true))
    {
        return false;
    }
    LBR::Utils::DelayMs(150);

    // BUSY stays high until the chip has left reset. A TCXO module that
    // was not declared in settings can sit here until this wait expires.
    if (!wait_busy() || !configure() || !in_standby_rc())
    {
        return false;
    }

    initialized = true;
    return true;
}

/**
 * @brief Send one LoRa packet and block until completion or failure.
 *
 * Enters standby, sets the packet length and IRQ mask, writes the payload,
 * starts TX, and polls for TxDone or Timeout. A 20-second host guard also
 * limits the polling loop when the millisecond tick is running.
 *
 * SX1262 Datasheet:
 * - Section 15.2 LoRa Packet Transmission
 * - Table 13-51 SetPacketParams
 * - Table 13-26 WriteBuffer
 * - Table 13-6 SetTx
 *
 * @param tx_buf Payload of 1 to 255 bytes.
 * @return true if TxDone is observed without Timeout; false for invalid
 *         length, a communication failure, or a radio/host timeout.
 * @pre init() has completed successfully.
 */
bool Sx1262::transmit(const std::span<const uint8_t>& tx_buf)
{
    // The SX1262 packet length field permits a maximum of 255 bytes.
    if (tx_buf.empty() || tx_buf.size() > 255)
    {
        return false;
    }

    // SetPacketParams, WriteBuffer, and SetTx are accepted only in
    // STDBY_RC, STDBY_XOSC, or FS. A previous receive may still be in RX.
    if (!set_standby(StdbyConfig::RC))
    {
        return false;
    }

    // TxDone is bit 0 and Timeout is bit 9. Those flags stay clear after
    // reset unless SetDioIrqParams unmasks them. DIO2 and DIO3 are left
    // unmapped: DIO2 may be the RF switch, and DIO3 may control the TCXO.
    if (!set_dio_irq(kIrqTxMask))
    {
        return false;
    }

    // Section 13.4.3 / Table 13-51. Explicit header mode still needs the
    // number of bytes to send. Preamble, CRC, and IQ come from settings
    // and must match the receiver. 12 symbols, not 65535.
    if (!set_packet_params(static_cast<uint8_t>(tx_buf.size())))
    {
        return false;
    }

    // Table 13-26 WriteBuffer: opcode, offset, then the payload. Offset 0
    // is the TX base address programmed by init().
    std::array<uint8_t, 257> buffer{};
    buffer[0] = command_byte(Opcode::WRITE_BUFFER);
    buffer[1] = 0x00;
    for (std::size_t i = 0; i < tx_buf.size(); ++i)
    {
        buffer[i + 2] = tx_buf[i];
    }
    std::span<uint8_t> frame(buffer.data(), tx_buf.size() + 2);
    if (!write_command(frame))
    {
        return false;
    }

    // A stale TxDone from the previous packet would look like success.
    if (!clear_irq(0xFFFF))
    {
        return false;
    }

    // SetTx timeout is a 24-bit count of 15.625 us steps. 0x000000 disables
    // the chip timeout; TxDone still fires when the modem finishes. The
    // host guard below only stops a radio that never starts from blocking
    // the flight computer.
    const uint32_t timeout = settings.tx_timeout;
    std::array<uint8_t, 4> set_tx{
        command_byte(Opcode::SET_TX),
        static_cast<uint8_t>((timeout >> 16) & 0xFFu),
        static_cast<uint8_t>((timeout >> 8) & 0xFFu),
        static_cast<uint8_t>(timeout & 0xFFu),
    };
    if (!write_command(set_tx))
    {
        return false;
    }

    // GetIrqStatus is a read command: opcode, then a NOP that shifts out
    // the status byte, then IrqStatus[15:0]. Poll until TxDone or Timeout.
    uint16_t irq = 0;
    const uint32_t start_ms = LBR::Utils::GetMsTicks();
    while ((irq & kIrqTxMask) == 0)
    {
        if ((LBR::Utils::GetMsTicks() - start_ms) > kHostGuardMs)
        {
            // SetStandby aborts a transmission that never raised TxDone.
            set_standby(StdbyConfig::RC);
            clear_irq(0xFFFF);
            return false;
        }
        if (!get_irq(irq))
        {
            return false;
        }
    }

    // ClearIrqStatus clears every bit that is 1 in this mask.
    if (!clear_irq(irq))
    {
        return false;
    }

    // Timeout is not success, even if TxDone is also set.
    return (irq & kIrqTxDone) != 0 && (irq & kIrqTimeout) == 0;
}

/**
 * @brief Receive one LoRa packet into the supplied buffer.
 *
 * Enters standby, starts RX, and polls for an RX event. For a valid packet,
 * reads its length and starting offset before copying it without truncation.
 * In implicit-header mode, the buffer size is the expected packet length.
 * A 20-second host guard limits polling when the millisecond tick is running.
 *
 * SX1262 Datasheet:
 * - Section 15.3 LoRa Packet Reception
 * - Table 13-8 SetRx
 * - Table 13-27 ReadBuffer
 * - Table 13-79 GetRxBufferStatus
 *
 * @param[out] rx_buf Destination for the payload; must not be empty.
 * @return Number of bytes copied, or 0 for an empty/oversized packet,
 *         timeout, header or CRC error, invalid buffer, or communication
 *         failure. A zero-length payload is therefore not distinguishable
 *         from an error.
 * @pre init() has completed successfully.
 */
std::size_t Sx1262::receive(const std::span<uint8_t>& rx_buf)
{
    // ReadBuffer cannot invent a length. An empty caller buffer cannot
    // hold a packet, so fail before touching the radio.
    if (rx_buf.empty())
    {
        return 0;
    }

    // Implicit header has no length field. The length programmed here has
    // to be the packet length, and it has to fit in one byte.
    uint8_t packet_length = 0xFF;
    if (settings.header == HeaderType::IMPLICIT)
    {
        if (rx_buf.size() > 255)
        {
            return 0;
        }
        packet_length = static_cast<uint8_t>(rx_buf.size());
    }

    // SetRx is issued from standby. Falling back first also stops a
    // previous continuous receive.
    if (!set_standby(StdbyConfig::RC))
    {
        return 0;
    }

    // In explicit mode, the payload-length byte is unused on RX; the real
    // length arrives in the packet header. In implicit mode, it is the
    // expected length and must match the transmitter.
    if (!set_packet_params(packet_length))
    {
        return 0;
    }

    // RxDone bit 1, HeaderErr bit 5, CrcErr bit 6, Timeout bit 9.
    // HeaderErr and CrcErr have to be unmasked or a bad packet looks like
    // RxDone with no way to reject it.
    if (!set_dio_irq(kIrqRxMask))
    {
        return 0;
    }
    if (!clear_irq(0xFFFF))
    {
        return 0;
    }

    // SetRx uses the same 15.625 us timeout unit as SetTx.
    // 0x000000 is single RX with the timeout disabled, so this poll would
    // wait until the host guard. 0xFFFFFF is continuous RX: the chip stays
    // in RX and the buffer pointer advances. A finite value is one packet,
    // then standby. 0x00FA00 is one second.
    const uint32_t timeout = settings.rx_timeout;
    std::array<uint8_t, 4> set_rx{
        command_byte(Opcode::SET_RX),
        static_cast<uint8_t>((timeout >> 16) & 0xFFu),
        static_cast<uint8_t>((timeout >> 8) & 0xFFu),
        static_cast<uint8_t>(timeout & 0xFFu),
    };
    if (!write_command(set_rx))
    {
        return 0;
    }

    uint16_t irq = 0;
    const uint32_t start_ms = LBR::Utils::GetMsTicks();
    while ((irq & kIrqRxMask) == 0)
    {
        if ((LBR::Utils::GetMsTicks() - start_ms) > kHostGuardMs)
        {
            // SetStandby leaves RX. Required when rx_timeout is 0, because
            // the chip timeout never fires and the host guard is the stop.
            set_standby(StdbyConfig::RC);
            clear_irq(0xFFFF);
            return 0;
        }
        if (!get_irq(irq))
        {
            return 0;
        }
    }

    // Datasheet RX procedure: RxDone alone is not enough. Discard the
    // payload when the header or the payload CRC failed. Timeout with no
    // RxDone means the window closed before a packet arrived.
    const bool header_and_crc_ok = (irq & (kIrqHeaderErr | kIrqCrcErr)) == 0;
    const bool packet_received_without_errors =
        (irq & kIrqRxDone) != 0 && header_and_crc_ok;

    uint8_t payload_len = 0;
    uint8_t start = 0;
    if (packet_received_without_errors)
    {
        // GetRxBufferStatus returns the length from the explicit header
        // and the offset of the first payload byte. The buffer is circular,
        // so that offset is not always 0 even when the RX base address is.
        std::array<uint8_t, 3> rx_status{};
        if (!read_command(command_byte(Opcode::GET_RX_BUFFER_STATUS),
                          rx_status))
        {
            clear_irq(irq);
            return 0;
        }
        // Bits 3:1 of the status byte. 3, 4, and 5 are command timeout,
        // processing error, and execution failure.
        const uint8_t cmd = static_cast<uint8_t>((rx_status[0] >> 1) & 0x07u);
        if (cmd == 0x03 || cmd == 0x04 || cmd == 0x05)
        {
            clear_irq(irq);
            return 0;
        }
        payload_len = rx_status[1];
        start = rx_status[2];
    }

    // Do not return a truncated packet. The caller has no length if this
    // function returns 0, so a short copy would look like success.
    const bool can_copy_payload = packet_received_without_errors &&
                                  payload_len > 0 &&
                                  payload_len <= rx_buf.size();
    bool payload_copied = false;

    // Copy the payload from the RX buffer if possible.
    if (can_copy_payload)
    {
        // Table 13-27 ReadBuffer: opcode, start offset, NOP, then payload.
        // The NOP is required. Data starts on the following clock, not in
        // the NOP byte. BUSY rises after GetRxBufferStatus ends, so wait
        // before starting this command.
        if (!wait_busy())
        {
            clear_irq(irq);
            return 0;
        }
        std::array<uint8_t, 3> read_header{
            command_byte(Opcode::READ_BUFFER),
            start,
            0x00,
        };
        std::span<uint8_t> payload(rx_buf.data(), payload_len);
        cs.cs_enable();
        const bool read_header_written = spi.write(read_header);
        const bool payload_read_succeeded =
            read_header_written && spi.read(payload);
        cs.cs_disable();
        payload_copied = payload_read_succeeded;
    }

    if (!clear_irq(irq == 0 ? 0xFFFF : irq))
    {
        return 0;
    }

    return payload_copied ? payload_len : 0;
}

/**
 * @brief Validate and store new settings, applying them if initialized.
 *
 * A failed application marks the driver uninitialized; call init() to retry.
 * If the driver is not initialized, the settings are stored for the next
 * init() without communicating with the radio.
 *
 * @param new_settings Replacement settings.
 * @return true if the settings pass validation and any application succeeds;
 *         false otherwise.
 */
bool Sx1262::set_settings(Sx1262Settings new_settings)
{
    if (!settings_ok(new_settings))
    {
        return false;
    }
    settings = new_settings;
    if (!initialized)
    {
        return true;
    }
    if (!configure() || !in_standby_rc())
    {
        initialized = false;
        return false;
    }

    return true;
}

/**
 * @brief Wait until the radio's BUSY pin goes low.
 * @return true when the radio is ready, or false after the host BUSY timeout.
 * @note The millisecond tick must advance for the timeout to expire.
 */
bool Sx1262::wait_busy()
{
    // BUSY high means the previous command is still executing. The host
    // must not send another command until it falls. WriteBuffer and
    // ReadBuffer leave BUSY low, so this returns immediately for those.
    const uint32_t start_ms = LBR::Utils::GetMsTicks();
    while (busy_pin.read())
    {
        if ((LBR::Utils::GetMsTicks() - start_ms) > kBusyTimeoutMs)
        {
            return false;
        }
    }

    return true;
}

/**
 * @brief Send a complete command frame under one chip-select assertion.
 * @param command Opcode followed by its command parameters.
 * @return true if the nonempty frame was written; false on BUSY timeout or
 *         SPI failure. This does not independently check radio command status.
 */
bool Sx1262::write_command(std::span<uint8_t> command)
{
    if (command.empty() || !wait_busy())
    {
        return false;
    }
    cs.cs_enable();
    const bool written = spi.write(command);
    cs.cs_disable();

    return written;
}

/**
 * @brief Send a read opcode and collect its status and response bytes.
 *
 * Chip select remains asserted across the SPI write and read. The first
 * response byte is the status byte; subsequent bytes are command data.
 *
 * @param command Read-command opcode.
 * @param[out] response Nonempty buffer for status and command data.
 * @return true if the SPI operations succeed; false for an empty buffer,
 *         BUSY timeout, or SPI failure. The caller interprets the status.
 */
bool Sx1262::read_command(uint8_t command, std::span<uint8_t> response)
{
    // Read-command format: opcode, then clocks that shift out the status
    // byte and the response. spi.read() drives MOSI low, which is the NOP
    // the datasheet shows after the opcode. response[0] is that status.
    if (response.empty() || !wait_busy())
    {
        return false;
    }
    std::array<uint8_t, 1> header{command};
    cs.cs_enable();
    const bool written = spi.write(header);
    const bool read_ok = written && spi.read(response);
    cs.cs_disable();

    return written && read_ok;
}

/**
 * @brief Write one to eight consecutive radio registers.
 * @param address Address of the first register.
 * @param data Register bytes to write.
 * @return true if the SPI frame is written; false on invalid length,
 *         BUSY timeout, or SPI failure.
 */
bool Sx1262::write_register(uint16_t address, std::span<const uint8_t> data)
{
    if (data.empty() || data.size() > 8 || !wait_busy())
    {
        return false;
    }
    std::array<uint8_t, 11> command{};
    command[0] = command_byte(Opcode::WRITE_REGISTER);
    command[1] = static_cast<uint8_t>(address >> 8);
    command[2] = static_cast<uint8_t>(address);
    for (std::size_t i = 0; i < data.size(); ++i)
    {
        command[i + 3] = data[i];
    }
    std::span<uint8_t> frame(command.data(), data.size() + 3);
    cs.cs_enable();
    const bool written = spi.write(frame);
    cs.cs_disable();

    return written;
}

/**
 * @brief Read one radio register.
 * @param address Register address.
 * @param[out] value Register value, updated only on a successful read.
 * @return true if the SPI read succeeds; false on BUSY timeout or SPI failure.
 */
bool Sx1262::read_register(uint16_t address, uint8_t& value)
{
    // ReadRegister: opcode, address MSB, address LSB, NOP, then data.
    // The NOP is not the data byte.
    if (!wait_busy())
    {
        return false;
    }
    std::array<uint8_t, 4> header{
        command_byte(Opcode::READ_REGISTER),
        static_cast<uint8_t>(address >> 8),
        static_cast<uint8_t>(address),
        0x00,
    };
    std::array<uint8_t, 1> data{};
    cs.cs_enable();
    const bool written = spi.write(header);
    const bool read_ok = written && spi.read(data);
    cs.cs_disable();
    if (!read_ok)
    {
        return false;
    }
    value = data[0];

    return true;
}

/**
 * @brief Request the selected standby oscillator mode.
 * @param mode RC or crystal-oscillator standby.
 * @return true if the command frame is written; false on communication failure.
 */
bool Sx1262::set_standby(StdbyConfig mode)
{
    // 0x80 is SetStandby, not GetStatus. GetStatus is 0xC0.
    std::array<uint8_t, 2> command{
        command_byte(Opcode::SET_STANDBY),
        static_cast<uint8_t>(mode),
    };

    return write_command(command);
}

/**
 * @brief Apply LoRa packet settings and the IQ-polarity register workaround.
 *
 * SX1262 Datasheet:
 * - Section 13.4.3 SetPacketParams
 * - Table 13-51
 * @param payload_length TX payload length or expected implicit RX length.
 * @return true if the command and register update succeed; false otherwise.
 */
bool Sx1262::set_packet_params(uint8_t payload_length)
{
    std::array<uint8_t, 7> command{
        command_byte(Opcode::SET_PACKET_PARAMS),
        static_cast<uint8_t>(settings.preamble_symbols >> 8),
        static_cast<uint8_t>(settings.preamble_symbols),
        static_cast<uint8_t>(settings.header),
        payload_length,
        static_cast<uint8_t>(settings.crc),
        static_cast<uint8_t>(settings.iq),
    };
    if (!write_command(command))
    {
        return false;
    }
    // Datasheet section 15.4: standard IQ needs bit 2 of 0x0736 set, and
    // inverted IQ needs it clear. The reset value is not enough once the
    // other polarity has been used.
    return fix_iq_polarity();
}

/**
 * @brief Enable the selected IRQs and map them to DIO1.
 *
 * DIO2 and DIO3 are left unmapped for possible RF-switch or TCXO control.
 *
 * SX1262 Datasheet:
 * - Section 13.5.1
 * - Table 13-29 (IRQ definitions)
 * - Table 13-31 (command format)
 *
 * @param mask Bit mask of IRQs to enable and route to DIO1.
 * @return true if the command frame is written; false on communication failure.
 */
bool Sx1262::set_dio_irq(uint16_t mask)
{
    std::array<uint8_t, 9> command{
        command_byte(Opcode::SET_DIO_IRQ_PARAMS),
        static_cast<uint8_t>(mask >> 8),
        static_cast<uint8_t>(mask),
        static_cast<uint8_t>(mask >> 8),
        static_cast<uint8_t>(mask),
        0x00,
        0x00,
        0x00,
        0x00,
    };

    return write_command(command);
}

/**
 * @brief Update the IQ-polarity register bit for the configured LoRa mode.
 *
 * SX1262 Datasheet:
 * - Section 15.4 LoRa Inverted IQ Operation
 * - Register 0x0736, bit 2
 *
 * @return true if the register read and write succeed; false otherwise.
 */
bool Sx1262::fix_iq_polarity()
{
    uint8_t value = 0;
    if (!read_register(kRegIqPolarity, value))
    {
        return false;
    }
    if (settings.iq == InvertIQ::INVERTED)
    {
        value = static_cast<uint8_t>(value & 0xFB);
    }
    else
    {
        value = static_cast<uint8_t>(value | 0x04);
    }
    std::array<uint8_t, 1> data{value};

    return write_register(kRegIqPolarity, data);
}

/**
 * @brief Update the TX modulation register for the configured bandwidth.
 *
 * SX1262 Datasheet:
 * - Section 15.1 Modulation Quality with 500 kHz LoRa Bandwidth
 * - Register 0x0889, bit 2
 *
 * @return true if the register read and write succeed; false otherwise.
 */
bool Sx1262::fix_modulation_quality()
{
    // Datasheet section 15.1. Bit 2 of 0x0889 must be set for LoRa
    // 500 kHz and cleared for every other bandwidth. SetModulationParams
    // does not do this, and the bit sticks across later calls.
    uint8_t value = 0;
    if (!read_register(kRegTxModulation, value))
    {
        return false;
    }
    if (settings.bandwidth == LoRaBandwidth::KHZ_500)
    {
        value = static_cast<uint8_t>(value | 0x04);
    }
    else
    {
        value = static_cast<uint8_t>(value & 0xFB);
    }
    std::array<uint8_t, 1> data{value};

    return write_register(kRegTxModulation, data);
}

/**
 * @brief Clear the radio IRQ flags selected by a bit mask.
 *
 * SX1262 Datasheet:
 * - Section 13.5.1
 * - Table 13-33 (IRQ clear)
 * @param mask A set bit clears the corresponding IRQ flag.
 * @return true if the command frame is written; false on communication failure.
 */
bool Sx1262::clear_irq(uint16_t mask)
{
    std::array<uint8_t, 3> command{
        command_byte(Opcode::CLR_IRQ_STATUS),
        static_cast<uint8_t>(mask >> 8),
        static_cast<uint8_t>(mask),
    };

    return write_command(command);
}

/**
 * @brief Read the radio's current IRQ flags.
 *
 * SX1262 Datasheet:
 * - Section 13.5.1
 * - Table 13-32 (IRQ status)
 * @param[out] irq IRQ bit mask, updated only on a successful SPI read.
 * @return true if the SPI read succeeds; false on BUSY timeout or SPI failure.
 */
bool Sx1262::get_irq(uint16_t& irq)
{
    std::array<uint8_t, 3> response{};
    if (!read_command(command_byte(Opcode::GET_IRQ_STATUS), response))
    {
        return false;
    }
    irq = static_cast<uint16_t>((static_cast<uint16_t>(response[1]) << 8) |
                                response[2]);
    return true;
}

/**
 * @brief Check the reported chip mode and command status.
 * @return true if the status indicates STDBY_RC without a reported command
 *         timeout or processing/execution error; false otherwise.
 */
bool Sx1262::in_standby_rc()
{
    // Status byte: bits 7:4 are the chip mode (0x2 = STDBY_RC) and bits
    // 3:1 are the command status. 0x3, 0x4, and 0x5 are timeout, processing
    // error, and execution failure.
    std::array<uint8_t, 1> response{};
    if (!read_command(command_byte(Opcode::GET_STATUS), response))
    {
        return false;
    }
    const uint8_t mode = static_cast<uint8_t>(response[0] & 0xF0);
    const uint8_t cmd = static_cast<uint8_t>((response[0] >> 1) & 0x07);

    return mode == 0x20 && cmd != 0x03 && cmd != 0x04 && cmd != 0x05;
}

/**
 * @brief Program the stored board and LoRa settings into the radio.
 *
 * Configures the oscillator control, regulator, RF switch, calibration,
 * frequency, PA, buffers, sync word, modulation, packet parameters, and IRQs.
 * Stops at the first failed command or register operation.
 *
 * @return true if all command and register operations succeed; false otherwise.
 * @note This helper does not reset the radio or check the final standby status.
 */
bool Sx1262::configure()
{
    // SetStandby(STDBY_RC) first. Calibration and the TCXO command are only
    // accepted in this mode. 0x80 is SetStandby; it is not GetStatus.
    if (!set_standby(StdbyConfig::RC))
    {
        return false;
    }

    // SetDio3AsTcxoCtrl must run before any command that starts the crystal
    // (Calibrate, SetRfFrequency). Skip it when the board uses the XTAL.
    if (settings.use_tcxo)
    {
        std::array<uint8_t, 5> tcxo{
            command_byte(Opcode::SET_DIO3_AS_TCXO_CTRL),
            static_cast<uint8_t>(settings.tcxo_voltage),
            static_cast<uint8_t>((settings.tcxo_timeout >> 16) & 0xFFu),
            static_cast<uint8_t>((settings.tcxo_timeout >> 8) & 0xFFu),
            static_cast<uint8_t>(settings.tcxo_timeout & 0xFFu),
        };
        if (!write_command(tcxo))
        {
            return false;
        }
    }

    std::array<uint8_t, 2> regulator{
        command_byte(Opcode::SET_REGULATOR_MODE),
        static_cast<uint8_t>(settings.use_dcdc ? 0x01 : 0x00),
    };
    if (!write_command(regulator))
    {
        return false;
    }

    // Enable this only for modules that route the PA through DIO2. If DIO2
    // is an IRQ pin, leave the setting false.
    std::array<uint8_t, 2> dio2{
        command_byte(Opcode::SET_DIO2_AS_RF_SWITCH),
        static_cast<uint8_t>(settings.dio2_as_rf_switch ? 0x01 : 0x00),
    };
    if (!write_command(dio2))
    {
        return false;
    }

    // Calibrate all blocks while still in STDBY_RC. 0x7F is every block.
    std::array<uint8_t, 2> calibrate{
        command_byte(Opcode::CALIBRATE),
        kCalibrateAll,
    };
    if (!write_command(calibrate))
    {
        return false;
    }

    // Reset default is GFSK. SetModulationParams and SetPacketParams are
    // rejected, or parsed as GFSK fields, until this is LoRa (0x01).
    // Datasheet Section 13.4.2, Table 13-37 SetPacketType.
    std::array<uint8_t, 2> packet_type{
        command_byte(Opcode::SET_PACKET_TYPE),
        kLoRaPacketType,
    };
    if (!write_command(packet_type))
    {
        return false;
    }

    // Datasheet Section 13.4.1, Table 13-36 SetRfFrequency.
    const uint32_t pll_word = frequency_to_pll_word(settings.frequency_hz);
    std::array<uint8_t, 5> frequency{
        command_byte(Opcode::SET_RF_FREQUENCY),
        static_cast<uint8_t>((pll_word >> 24) & 0xFFu),
        static_cast<uint8_t>((pll_word >> 16) & 0xFFu),
        static_cast<uint8_t>((pll_word >> 8) & 0xFFu),
        static_cast<uint8_t>(pll_word & 0xFFu),
    };
    if (!write_command(frequency))
    {
        return false;
    }

    // Image calibration has to follow SetRfFrequency and use the band that
    // contains it. 915 MHz uses 0xE1, 0xE9 (902-928 MHz).
    const ImageCal image = image_cal_for(settings.frequency_hz);
    std::array<uint8_t, 3> calibrate_image{
        command_byte(Opcode::CALIBRATE_IMAGE),
        image.freq1,
        image.freq2,
    };
    if (!write_command(calibrate_image))
    {
        return false;
    }

    // deviceSel 0x00 selects the SX1262 high-power PA. 0x01 is the SX1261
    // and will not produce +22 dBm. paLut must be 0x01.
    // Datasheet Section 13.1.14, Table 13-20 SetPaConfig; Section 15.2.
    std::array<uint8_t, 5> pa_config{
        command_byte(Opcode::SET_PA_CONFIG),
        settings.pa_duty_cycle,
        settings.hp_max,
        kSx1262DeviceSel,
        kPaLut,
    };
    if (!write_command(pa_config))
    {
        return false;
    }

    // Datasheet Section 13.4.4, Table 13-40 SetTxParams.
    std::array<uint8_t, 3> tx_params{
        command_byte(Opcode::SET_TX_PARAMS),
        static_cast<uint8_t>(settings.tx_power_dbm),
        settings.ramp_time,
    };
    if (!write_command(tx_params))
    {
        return false;
    }

    // +22 dBm needs about 140 mA. 0x38 is that SX1262 trim.
    std::array<uint8_t, 1> ocp{settings.ocp_trim};
    if (!write_register(kRegOcp, ocp))
    {
        return false;
    }

    // TX and RX both start at offset 0. transmit() writes there, and
    // receive() still uses GetRxBufferStatus because the RX buffer wraps.
    std::array<uint8_t, 3> base_address{
        command_byte(Opcode::SET_BUFFER_BASE_ADDRESS),
        0x00,
        0x00,
    };
    if (!write_command(base_address))
    {
        return false;
    }

    // LoRa sync word is two registers, not a command. 0x1424 is private.
    std::array<uint8_t, 2> sync_word{
        static_cast<uint8_t>(settings.sync_word >> 8),
        static_cast<uint8_t>(settings.sync_word),
    };
    if (!write_register(kRegSyncWord, sync_word))
    {
        return false;
    }

    // Datasheet Section 13.4.5, Table 13-42 SetModulationParams.
    const bool ldro =
        settings.low_data_rate_optimize || ldro_required(settings);
    std::array<uint8_t, 5> modulation{
        command_byte(Opcode::SET_MODULATION_PARAMS),
        settings.spreading_factor,
        static_cast<uint8_t>(settings.bandwidth),
        static_cast<uint8_t>(settings.coding_rate),
        static_cast<uint8_t>(ldro ? 0x01 : 0x00),
    };
    if (!write_command(modulation) || !fix_modulation_quality())
    {
        return false;
    }

    // Placeholder length. transmit() overwrites it with the real size.
    // Explicit RX ignores it and reads the length from the header.
    if (!set_packet_params(0xFF))
    {
        return false;
    }

    // Stop the RX timeout when a preamble is seen, so a long packet is not
    // aborted after the radio has already locked.
    std::array<uint8_t, 2> stop_timer{
        command_byte(Opcode::STOP_TIMER_ON_PREAMBLE),
        0x01,
    };
    if (!write_command(stop_timer))
    {
        return false;
    }

    // After TxDone or RxDone, return to STDBY_RC (0x20) rather than FS.
    // Datasheet Section 13.1.15, Table 13-22 SetRxTxFallbackMode.
    std::array<uint8_t, 2> fallback{
        command_byte(Opcode::SET_RX_TX_FALLBACK_MODE),
        kFallbackStandbyRc,
    };
    if (!write_command(fallback))
    {
        return false;
    }

    const uint16_t irq_mask = kIrqTxMask | kIrqRxMask;

    return set_dio_irq(irq_mask);
}

}  // namespace LBR

#include "sx1262.h"
#include <span>
#include "gpio_cs.h"
#include "sx1262_enums.h"
#include "delay.h"
#include <array>
namespace LBR
{
Sx1262::Sx1262(Sx1262Params& params_)
    : settings{params_.settings}, spi{params_.spi}
{
}
    bool Sx1262::init()
    {
        cs.cs_disable();  // NSS high

        reset_pin.set(0);  // NREST low
        DelayMs(150);
        reset_pin.set(1);  // NREST High
        DelayMs(150);

        // wait until busy is low
        while(busy_pin.read());
        
        // Send the GetStatus command to check the status of the device
        std::array<uint8_t, 2> command{
            0x80,   // GetStatus opcode
            static_cast<uint8_t>(StdbyConfig::RC)    // STDBY_RC
        }; 
        cs.cs_enable(); // NSS low      
        spi.write(std::span<uint8_t>(command.data(), command.size()));
        cs.cs_disable();    // NSS high

        return true;
    }

    bool Sx1262::transmit(const std::span <const uint8_t> & tx_buf)
    {
        // Write the WriteBuffer Function page 83, filling the Sx1262 buffer with the data to transmit
        if (tx_buf.empty() || tx_buf.size() > 255)
        {
            return false;
        }

        if (busy_pin.read())
        {
            return false;
        }

        // Table 13-26: WriteBuffer command
        std::array<uint8_t, 257> buffer{
            0x0E, // WriteBuffer opcode
            0     // Offset within the buffer
        };

        // Data@offset, Table 13-26
        for (uint8_t i = 0; i < tx_buf.size(); i++)
        {
            buffer[i+2] = tx_buf[i]; 
        }
        
        // Pass the buffer to the SPI interface
        cs.cs_enable(); // NSS low
        spi.write(std::span<uint8_t>(buffer.data(), tx_buf.size() + 2));
        cs.cs_disable(); // NSS high

        constexpr uint16_t preambleLength = 0xFFFF;  // 65535

        // Configure packet parameters for transmission, Table 13-51
        std::array<uint8_t, 7> packet_params_command{
            0x8C,   // SetPacketParams opcode
            static_cast<uint8_t>(preambleLength >> 8),  // 0x01
            static_cast<uint8_t>(preambleLength),   // 0xFF
            static_cast<uint8_t>(HeaderType::EXPLICIT),   // HeaderType
            static_cast<uint8_t>(tx_buf.size()),         // PayloadLength
            static_cast<uint8_t>(CrcType::ON),          // CRC type
            static_cast<uint8_t>(InvertIQ::STANDARD)    // Invert IQ
        };

        cs.cs_enable(); // NSS low
        spi.write(std::span<uint8_t>(packet_params_command.data(), packet_params_command.size()));
        cs.cs_disable(); // NSS high

        std::array<uint8_t, 3> clear_irq_command{0x02, 0xFF, 0xFF};
        cs.cs_enable(); // NSS low
        spi.write(std::span<uint8_t>(clear_irq_command.data(), clear_irq_command.size()));
        cs.cs_disable(); // NSS high

        // Issue SetTx command to start transmission
        // Timeout duration = Timeout * 15.625 µs (per SX1262 datasheet)
        std::array<uint8_t, 3> set_tx_command{
            0x83,   // SetTx opcode
            0x00,   // Timeout byte 1 (to be configured)
            0x00,   // Timeout byte 2 (to be configured)
            0x00    // Timeout byte 3 (to be configured)
        };
        cs.cs_enable(); // NSS low
        spi.write(std::span<uint8_t>(set_tx_command.data(), set_tx_command.size()));
        cs.cs_disable(); // NSS high

        // GetIrqStatus: opcode, then Status, IrqStatus[15:8], IrqStatus[7:0].
        // TxDone is bit 0, Timeout is bit 9.
        std::array<uint8_t, 1> get_irq_status_command{0x12};
        std::array<uint8_t, 3> irq_status{};
        uint16_t irq = 0;

        // Wait for TxDone or Timeout
        while ((irq & 0x0201) == 0)
        {
            while (busy_pin.read())
            {
            }

            cs.cs_enable(); // NSS low
            spi.write(std::span<uint8_t>(get_irq_status_command.data(), get_irq_status_command.size()));
            spi.read(std::span<uint8_t>(irq_status.data(), irq_status.size()));
            cs.cs_disable(); // NSS high

            // Combine the two bytes of IRQ status into a single 16-bit value
            irq = (static_cast<uint16_t>(irq_status[1]) << 8) | irq_status[2];  
        }

        // Clears an IRQ flag in the IRQ register by setting to 1 the bit of ClearIrqParam 
        // corresponding to the same position as the IRQ flag to be cleared
        std::array<uint8_t, 3> clear_irq_command{
            0x02,   // ClearIrqStatus opcode
            static_cast<uint8_t>(irq >> 8), // IRQ status high byte
            static_cast<uint8_t>(irq) // IRQ status low byte
        };
        cs.cs_enable(); // NSS low
        spi.write(std::span<uint8_t>(clear_irq_command.data(), clear_irq_command.size()));
        cs.cs_disable(); // NSS high

        return true;
    }

    bool Sx1262::receive(const std::span <uint8_t> & rx_buf)
    {
        if (rx_buf.empty() || rx_buf.size() > 255)
        {
            return false;
        }

        if (busy_pin.read())
        {
            return false;
        }

        // Table 13-27: ReadBuffer command
        std::array<uint8_t, 3> buffer{
            0x1E, // ReadBuffer opcode
            0,     // Offset within the buffer
            0x00 // NOP must be sent after sending the offset
        }; 

        cs.cs_enable(); // NSS low
        spi.write(std::span<uint8_t>(buffer.data(), buffer.size()));
        spi.read(std::span<uint8_t>(rx_buf.data(), rx_buf.size()));
        cs.cs_disable(); // NSS high

        return true;
    }

    bool Sx1262::set_settings(Sx1262Settings new_settings)
    {
        if (!new_settings)
        {
            return false;
        }
        settings = new_settings;
        return true;
    }

}  // namespace LBR

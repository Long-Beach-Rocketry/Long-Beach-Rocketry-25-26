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
    bool Sx1262::init(){
        cs.cs_disable();  //NSS high
        reset_pin.set(0);  // NREST low
        delay(150);
        reset_pin.set(1);  //NREST High
        delay(150);
        //wait unitl busy is low
        while(busy_pin.read());
        
        //NSS low
        cs.cs_enable();
        //
        std::array<uint8_t, 2> command {};
        command[0] = 0x80;
        command[1] = 0x00;
        spi.write(std::span<uint8_t>(command.data(), command.size()));
        cs.cs_disable();

        return true;
    }

    bool Sx1262::transmit(const std::span <const uint8_t> & tx_buf){
        //write the WriteBuffer Function page 83.
        if(tx_buf.empty() || tx_buf.size() > 255){
            return false;
        }

        if(busy_pin.read()){
            return false;
        }
        //opcode: mean write the buffer. 
        std::array<uint8_t, 257> buffer{}; 
        buffer[0] =0x0E; 
        buffer[1] = 0; //it is offset

        for(uint8_t  i = 0; i < tx_buf.size(); i++){
            // pass all the data set
            buffer[i+2] = tx_buf[i]; 
        }
        cs.cs_enable();
        
        // pass to the spi
        spi.write(std::span<uint8_t>(buffer.data(), tx_buf.size() + 2));
        cs.cs_disable();
        constexpr uint16_t preambleLength = 65535;  

        std::array<uint8_t, 7> packet_params_command{
            0x8C,
            static_cast<uint8_t>(preambleLength >> 8),
            static_cast<uint8_t>(preambleLength),
            static_cast<uint8_t>(HeaderType::EXPLICIT),
            static_cast<uint8_t>(tx_buf.size()),
            static_cast<uint8_t>(CrcType::ON),
            static_cast<uint8_t>(InvertIQ::STANDARD)
        };

        cs.cs_enable();
        spi.write(std::span<uint8_t>(packet_params_command.data(), packet_params_command.size()));
        cs.cs_disable();

        std::array<uint8_t, 3> clear_irq_command{0x02, 0xFF, 0xFF};
        cs.cs_enable()
        spi.write(std::span<uint8_t>(clear_irq_command.data(), clear_irq_command.size()));
        cs.cs_disable();

        // TODO: Issue SetTx with a configured timeout.

        // TODO: Check TxDone or timeout using GetIrqStatus.

        // TODO: Clear handled IRQ flags.

        return false;
    }

    bool Sx1262::receive(const std::span <const uint8_7> & tx_buf){

    }

    bool Sx1262::set_settings(){

    }

}  // namespace LBR

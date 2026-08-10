#include "inv_bno055.h"

namespace LBR
{

InvBno055::InvBno055(uint8_t dev_addr)
    : dev_addr_{dev_addr}, recent_reg_{Bno055_CHIP_ID_REG}
{
}

bool InvBno055::process_read(const uint8_t reg_addr, uint8_t& data)
{
    recent_reg_ = reg_addr;

    // This is where the big switch case tree will be.
    // TODO: Finish this part for all register addresses

    switch (recent_reg_)
    {
        case Bno055_CHIP_ID_REG:
            data = CHIP_ID;
            recent_reg_++;
            return true;

        default:
            /*
                Returns 0x00 on reading invalid address and sends an ACK
                so doesn't actually fail?
            */
            data = Bn055_NULL_BYTE;
            return true;
    }
    return false;
}

bool InvBno055::process_read(uint8_t& data)
{
    return process_read(recent_reg_, data);
}

bool InvBno055::process_write(const uint8_t reg_addr, uint8_t data)
{

    recent_reg_ = reg_addr;
    // Another big switch case tree but for write.
    // TODO: Finish this.

    switch (recent_reg_)
    {
        /* For example, Bno055 PWR_REG is 0x3E */
        case 0x3E:
            // This would be a global variable instead simulating a register.
            uint8_t Bno055_PWR_REG = data;
            recent_reg_++;
            return true;

        default:
            return false;
    }
    return false;
}

bool InvBno055::process_write(uint8_t data)
{
    return process_write(recent_reg_, data);
}

bool InvBno055::get_8bit_addr(uint8_t& addr)
{
    addr = dev_addr_;
    return true;
}

}  // namespace LBR
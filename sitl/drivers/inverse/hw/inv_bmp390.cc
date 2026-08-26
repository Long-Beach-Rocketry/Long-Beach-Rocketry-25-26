#include "inv_bmp390.h"

namespace LBR
{

InvBmp390::InvBmp390(uint8_t dev_addr)
    : dev_addr_{dev_addr}, recent_reg_{Bmp390_CHIP_ID_REG}
{
}

bool InvBmp390::process_read(const uint8_t reg_addr, uint8_t& data)
{
    // Big switch case tree again
    switch (reg_addr)
    {
        case Bmp390_CHIP_ID_REG:
            data = CHIP_ID;
            recent_reg_ = Bmp390_CHIP_ID_REG;
            return true;

        default:
            return false;
    }

    return false;
}

bool InvBmp390::process_read(uint8_t& data)
{
    return process_read(recent_reg_, data);
}

bool InvBmp390::process_write(const uint8_t reg_addr, uint8_t data)
{
    // Another big switch case tree but for write.
    // TODO: Finish this.

    switch (reg_addr)
    {
        /* For example, Bno055 PWR_REG is 0x3E */
        case Bmp390_PWR_CTRL_REG:
            // This would be a global variable instead simulating a register.
            uint8_t Bno055_PWR_REG = data;
            recent_reg_ = Bmp390_PWR_CTRL_REG;
            return true;

        default:
            return false;
    }
    return false;
}

bool InvBmp390::process_write(uint8_t data)
{
    return process_write(recent_reg_, data);
}

bool InvBmp390::get_8bit_addr(uint8_t& addr)
{
    addr = dev_addr_;
    return true;
}
}  // namespace LBR
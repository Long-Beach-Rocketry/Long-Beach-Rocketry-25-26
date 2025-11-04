#include "reg_helpers.h"

/**
 * @brief Sets any SPI register with the desired bits
 * 
 * @param reg 
 * @param enum_val 
 * @param bit_num 
 * @param bit_length 
 */
void SetReg(volatile uint32_t* reg, uint32_t enum_val, uint32_t bit_num,
            uint32_t bit_length)
{
    uint32_t mask{(0x01 << bit_length) - 1U};
    *reg &= ~(mask << bit_num);
    *reg |= (mask & enum_val) << bit_num;
}
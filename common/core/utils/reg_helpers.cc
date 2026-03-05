<<<<<<< HEAD
#include "reg_helpers.h"

void SetReg(volatile uint32_t* reg, uint32_t enum_val, uint32_t bit_num,
            uint32_t bit_length)
{
    uint32_t mask{(0x01 << bit_length) - 1U};
    *reg &= ~(mask << bit_num);
    *reg |= (mask & enum_val) << bit_num;
}

uint16_t combine_uint16(uint8_t msb, uint8_t lsb)
{
    return (static_cast<uint16_t>(msb) << 8) | static_cast<uint16_t>(lsb);
}

int16_t combine_int16(uint8_t msb, uint8_t lsb)
{
    return (static_cast<int16_t>(msb) << 8) | static_cast<int16_t>(lsb);
=======
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
>>>>>>> 319d772 (ekf)
}
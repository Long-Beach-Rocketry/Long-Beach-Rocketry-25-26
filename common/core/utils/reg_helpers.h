#pragma once
#include <cstdint>

/**
 * @brief Sets any register with the desired bits
 * 
 * @param reg Pointer to the register to be modified
 * @param enum_val Value to write into a bit-field
 * @param bit_num Starting bit position of bit-field
 * @param bit_length Length of bit-field
 */
void SetReg(volatile uint32_t* reg, uint32_t enum_val, uint32_t bit_num,
            uint32_t bit_length);

/**
 * @brief Combines two bytes into a 16-bit value
 * @param msb Byte that will become the most significant bits
 * @param lsb Byte that will become the least significant bits
 * @return 16-bit unsigned integer
 */
uint16_t combine_uint16(uint8_t msb, uint8_t lsb);

/**
 * @brief Combines two bytes into a 16-bit signed value
 * @param msb Byte that will become the most significant bits
 * @param lsb Byte that will become the least significant bits
 * @return 16-bit signed integer
 */
int16_t combine_int16(uint8_t msb, uint8_t lsb);
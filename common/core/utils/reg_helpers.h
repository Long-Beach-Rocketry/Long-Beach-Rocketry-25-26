#pragma once
#include <cstdint>

// Declaration for SetReg implemented in reg_helpers.cc
void SetReg(volatile uint32_t* reg, uint32_t enum_val, uint32_t bit_num,
            uint32_t bit_length);
/**
 * @file board.h
 * @brief CRC driver interface
 * @author Joseph Chang
 */

#pragma once
#include "crc.h"
#include "gpio.h"
#include "usart.h"

#include <vector>

extern uint8_t rxb;

namespace LBR
{

struct Board
{
    Crc& crc;

    Usart& usart;

    Gpio& rx;
    Gpio& tx;
};

bool bsp_init(void);
Board& get_board(void);

std::vector<uint8_t> simulate_noise(std::span<const uint8_t> data,
                                    double dropProbability,
                                    double bitFlipProbability);

}  // namespace LBR
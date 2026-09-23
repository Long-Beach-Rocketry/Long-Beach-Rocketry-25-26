#pragma once

#include "usart_pipe.h"
#include "rs485.h"
#include "w25q.h"
// #include "lora.h"

namespace LBR
{

struct Board
{
    Pipeline& pipeline;
    Rs485& rs485;
    W25q& flash;
    // LoRa& lora;

    bool init();
};

Board& get_board();

}  // namespace LBR
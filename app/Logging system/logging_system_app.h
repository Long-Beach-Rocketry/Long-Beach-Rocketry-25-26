// saladin

#pragma once
#include "crc.h"
#include "gpio.h"
#include "rs485.h"
#include "usart.h"
#include "usart_pipe.h"
#include "st_usart.h"



//hardware specific initialization for the usart_pipe_test application
void st_usart_init(uint32_t sys_clck, uint32_t baud_rate, LBR::Stmh7::OversampleMode mode = LBR::Stmh7::OversampleMode::BY_16);

//transmit specific initialization for the usart_pipe_test application
void st_usart_send_string(const char* str, LBR::Stmh7::OversampleMode mode = LBR::Stmh7::OversampleMode::BY_16);

enum class LogLevel {
    DEBUG, INFO, WARNING, ERROR, CRITICAL
};

std::string format_log_message(LogLevel level, const char* message);


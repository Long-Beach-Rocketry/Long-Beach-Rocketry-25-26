/**
* @brief Logger module for logging messages for telemetry.
*/
#pragma once

#include "usart.h"

namespace LBR
{
class Logger
{
private:
    Usart* usart;
    static const std::size_t MAX_LOG_MESSAGE_LENGTH = 128;

public: 

    /**
     * @brief Initializes the logger with USART
     * @param usart_base The base address of the USART peripheral
     */

    Logger(USART_TypeDef* usart_base);

    bool log_debug(const char* message);

    char log_error(const char* message);

    char log_warn(const char* message);

    char log_info(const char* message);

    bool size_check(const char* message);

    void flash_logger_print(void);
};

}


/**
* @brief Logger module for logging messages for telemetry.
*/
#pragma once

#include <cstddef>
#include "usart.h"

namespace LBR
{
class Logger
{
private:
    Usart& uart;
    static const std::size_t MAX_LOG_MESSAGE_LENGTH = 128;

    /*
     * @brief Checks if size is valid with MAX_LOG_MESSAGE_LENGTH
     *
     * @param message Type const char* 
     */    
    bool size_check(const char* message) const;

    /*
     * @brief Formats and sends message for individual function call
     *
     * @param prefix LOG prefix eg. "LOG DEBUG >> : "
     * @param message Type const char* 
     */    
    bool format_and_send(const char* prefix, const char* message);

public: 

    /*
     * @brief Initializes the logger with USART
     *
     * @param uart The base address of the USART peripheral
     */

    Logger(Usart& uart);

    /*
     * @brief Convert string into char buffer
     *
     * @param message Type const char* 
     */
    bool log_debug(const char* message);
    bool log_error(const char* message);
    bool log_warn(const char* message);
    bool log_info(const char* message);

    /*
     * @brief (Feature) Prints all logs 
     *
     * @param message Type const char* 
     */
    bool print_all(const char* message);
};

}


/**
* @file logger.h
* @brief Logger module for logging messages for telemetry.
* @author Aaron Oh
*/
#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>
#include "usart.h"

namespace LBR
{
class Logger
{
private:
    Usart& _usart;
    static const std::size_t MAX_LOG_MESSAGE_LENGTH = 64;

    /**
     * @brief Checks if size is valid with MAX_LOG_MESSAGE_LENGTH
     *
     * @param message Type const char* 
     * @return true/false if the message is less than MAX_LOG_MESSAGE_LENGTH
     */
    bool size_check(const char* message) const;

    /**
     * @brief Formats and sends message for individual function call
     *
     * @param prefix LOG prefix eg. "LOG DEBUG >> : "
     * @param message Type const char* 
     */
    bool format_and_send(const char* prefix, const char* message);

public:

    /**
     * @brief Initializes the logger with USART
     *
     * @param uart The base address of the USART peripheral
     */
    Logger(Usart& uart);

    /**
     * @brief Convert string into char buffer for debug
     *
     * @param message Type const char* 
     */
    bool log_debug(const char* message);

    /**
     * @brief Convert string into char buffer for error
     *
     * @param message Type const char* 
     */
    bool log_error(const char* message);

    /**
     * @brief Convert string into char buffer for warn
     *
     * @param message Type const char* 
     */
    bool log_warn(const char* message);

    /**
     * @brief Convert string into char buffer for info
     *
     * @param message Type const char* 
     */
    bool log_info(const char* message);

};

}  // namespace LBR

#include "logger.h"

namespace LBR
{
// Initialize Logger by reference, only need to use send() in Uart
Logger::Logger(Usart& uart) 
    :usart(uart){}

bool Logger::log_debug(const char* message)
{
    return format_and_send("LOG DEBUG >> ", message);
}

bool Logger::log_error(const char* message)
{
    return format_and_send("LOG ERROR >> ", message);
}

bool Logger::log_warn(const char* message)
{
    return format_and_send("LOG WARN  >> ", message);
}

bool Logger::log_info(const char* message)
{
    return format_and_send("LOG INFO  >> ", message);
}

bool Logger::size_check(const char* message) const
{
    return (message != nullptr && std::strlen(message) <= MAX_LOG_MESSAGE_LENGTH);
}   

// handles the string buff and prepend logic
bool Logger::format_and_send(const char* prefix, const char* message)
{
    if(!size_check(message)) 
    {
        return false;
    }

    // Stack allocation.  18 = 15 (prefix) + 2 (\r\n) + 1 (\0)
    char tx_buffer[MAX_LOG_MESSAGE_LENGTH + 18]; 
    
    int written = std::snprintf(tx_buffer, sizeof(tx_buffer), 
                                "%s%s\r\n", 
                                prefix, message);
    
    if (written < 0 || written >= static_cast<int>(sizeof(tx_buffer)))
    {
        return false;
    }

    // Cleans format
    return uart.send(std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(tx_buffer),
                                                static_cast<std::size_t>(written)));
}

bool Logger::print_all(const char* message)
{
    if (!size_check(message))
    {
        return false;
    }

    char buffer[720];

    const int written = std::snprintf(buffer, sizeof(buffer),
        "--------------------------------\r\n"
        " LOGGER\r\n"
        "--------------------------------\r\n"
        " LOG DEBUG >> : %s \r\n"
        " LOG ERROR >> : %s \r\n"
        " LOG WARN  >> : %s \r\n"
        " LOG INFO  >> : %s \r\n"
        "--------------------------------\r\n",
        message, message, message, message);

    if (written < 0 || written >= static_cast<int>(sizeof(buffer)))
    {
        return false;
    }

    return uart.send(std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(buffer),
                                                static_cast<std::size_t>(written)));
}

} 
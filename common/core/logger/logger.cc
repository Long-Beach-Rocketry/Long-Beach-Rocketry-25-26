#include "logger.h"

namespace LBR
{
// Initialize Logger by reference, only need to use send() in Uart
Logger::Logger(Usart& uart) : _usart(uart)
{
}

bool Logger::log_debug(const char* message)
{
    return format_and_send("\033[1;3;4;7;36;40mLOG DEBUG >>\033[0m ", message);
}

bool Logger::log_error(const char* message)
{
    return format_and_send("\033[1;3;4;31;40mLOG ERROR >>\033[0m ", message);
}

bool Logger::log_warn(const char* message)
{
    return format_and_send("\033[1;3;4;7;33;40mLOG WARN >>\033[0m ", message);
}

bool Logger::log_info(const char* message)
{
    return format_and_send("\033[1;3;4;35;40mLOG INFO >>\033[0m ", message);
}

bool Logger::size_check(const char* message) const
{
    // const std::size_t length = std::strlen(message);
    // return (message != nullptr && length <= MAX_LOG_MESSAGE_LENGTH);
    if (message == nullptr)
    {
        return false;
    }

    return strnlen(message, MAX_LOG_MESSAGE_LENGTH + 1) <=
           MAX_LOG_MESSAGE_LENGTH;
}

// handles the string buff and prepend logic
bool Logger::format_and_send(const char* prefix, const char* message)
{
    if (!size_check(message))
    {
        return false;
    }

    // mem allocation 18 = 15 (prefix) + 2 (\r\n) + 1 (\0).
    char tx_buffer[MAX_LOG_MESSAGE_LENGTH + 18];

    int written = std::snprintf(tx_buffer, sizeof(tx_buffer), "%s%s\r\n",
                                prefix, message);

    if (written < 0 || written >= static_cast<int>(sizeof(tx_buffer)))
    {
        return false;
    }

    // Cleans format
    return usart.send(
        std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(tx_buffer),
                                 static_cast<std::size_t>(written)));
}

}  // namespace LBR
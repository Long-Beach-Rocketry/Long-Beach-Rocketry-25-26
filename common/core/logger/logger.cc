#include "logger.h"

namespace LBR
{
Logger::Logger(USART_TypeDef* usart_base) 
    :usart(usart_base)
{
}

bool Logger::log_debug(const char* message)
{
    return usart->send(message_compress(message)), strlen(message);
    // return ("LOG DEBUG >> " + std::string(message) + "\n");
}

char Logger::log_error(const char* message)
{
    return usart->send(message_compress(message)), strlen(message);
    // return ("LOG ERROR >> " + std::string(message) + "\n");
}

char Logger::log_warn(const char* message)
{
    return usart->send(message_compress(message)), strlen(message);
    // return ("LOG WARN >> " + std::string(message) + "\n");
}

char Logger::log_info(const char* message)
{
    return usart->send(message_compress(message)), strlen(message);
    // return ("LOG INFO >> " +std::string(message) + "\n");
}
bool Logger::size_check(const char* message)
{
    if (len < 0)
        {
            continue;
        }

        if (len >= static_cast<int>(sizeof(buf)))
        {
            len = static_cast<int>(sizeof(buf)) - 1;
        }
        //change type and send tx_span to uart
        std::span<const uint8_t> tx_span(reinterpret_cast<const uint8_t*>(buf),
                                         static_cast<size_t>(len));
        board.usart.send(tx_span);
}

void flash_logger_print(void){
    int len = snprintf(buf, sizeof(buf),
                           "--------------------------------\r\n"
                           " LOGGER\r\n"
                           "--------------------------------\r\n"
                           " LOG DEBUG >> : %s \r\n"
                           " LOG ERROR >> : %s \r\n"
                           " LOG WARN  >> : %s \r\n"
                           " LOG INFO  >> : %s \r\n"
                           
                           "--------------------------------\r\n",
                           message, message, message, message);
                        }
        
}


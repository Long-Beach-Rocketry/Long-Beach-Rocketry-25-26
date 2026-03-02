#include "usart_pb_helper.h"

namespace LBR
{

bool send_cmd_msg(const CmdMessage* msg, Usart& usart)
{
    // Fixed buffer size for simplicity
    uint8_t buffer[128];

    // Messages should not exceed buffer size
    int len = msg->encode(buffer, sizeof(buffer));
    if (len <= 0)
    {
        return false;
    }
    // Send the encoded message over USART
    return usart.send(std::span<const uint8_t>(buffer, len));
}

bool receive_cmd_msg(CmdMessage* msg, Usart& usart, size_t maxlen)
{
    // Fixed buffer size for simplicity (same)
    uint8_t buffer[128];

    // Read bytes from USART until we have a complete message or reach maxlen
    size_t count = 0;
    for (; count < maxlen; ++count)
    {
        uint8_t byte;
        if (usart.receive(byte))
        {
            buffer[count] = byte;
        }
        else
        {
            break;  // Otherwise, we assume no more data is available for now
        }
    }
    if (count == 0)
        return false;
    // Decode the received bytes into a CmdMessage object
    return msg->decode(buffer, count);
}

}  // namespace LBR

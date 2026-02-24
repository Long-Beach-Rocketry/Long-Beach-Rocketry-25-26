#include "usart_pb_helper.h"

namespace LBR
{

bool send_cmd_msg(const CmdMessage* msg, Usart& usart)
{
    uint8_t buffer[128];
    int len = msg->encode(buffer, sizeof(buffer));
    if (len <= 0)
        return false;
    return usart.send(std::span<const uint8_t>(buffer, len));
}

bool receive_cmd_msg(CmdMessage* msg, Usart& usart, size_t maxlen)
{
    uint8_t buffer[128];
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
            break;
        }
    }
    if (count == 0)
        return false;
    return msg->decode(buffer, count);
}

}  // namespace LBR

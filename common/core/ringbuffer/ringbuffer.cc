#include "ringbuffer.h"

namespace LBR
{
RingBuffer::RingBuffer(uint8_t* const buffer_, const size_t max_len_)
    : buffer{buffer_}, head{0}, tail{0}, curr{0}, max_len{max_len_}
{
}

int RingBuffer::push(uint8_t data)
{
    size_t next = (head + 1) % max_len;

    if (curr == max_len)
    {
        return 0;
    }

    buffer[head] = data;
    head = next;
    curr++;
    return 1;
}

int RingBuffer::pop(uint8_t* data)
{
    size_t next;

    next = (tail + 1) % max_len;

    if (curr == 0)
    {
        return 0;
    }

    *data = buffer[tail];
    tail = next;
    curr--;
    return 1;
}
}  // namespace LBR
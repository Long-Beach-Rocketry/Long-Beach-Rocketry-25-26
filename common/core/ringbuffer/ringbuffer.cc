#include "ringbuffer.h"

ringbuffer::ringbuffer(uint8_t* const buffer, const std::size_t max_len)
    : buffer{buffer}, head{0}, tail{0}, curr{0}, max_len{max_len} {};

int ringbuffer::ringbuffer_push(uint8_t data)
{
    std::size_t next = (head + 1) % max_len;

    if (curr == max_len)
    {
        return 0;
    }

    buffer[head] = data;
    head = next;
    curr++;
    return 1;
}

int ringbuffer::ringbuffer_pop(uint8_t* data)
{
    std::size_t next;

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

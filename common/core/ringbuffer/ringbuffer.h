/**
 * @file ringbuffer.h
 * @brief Ring Buffer Interface
 * @author Jaden Guidangen
 * @date 11/30/2025
 */

#pragma once
#include <cstdint>

class ringbuffer
{
public:
    explicit ringbuffer(uint8_t* const buffer, const std::size_t max_len);

    virtual int ringbuffer_push(uint8_t data);
    virtual int ringbuffer_pop(uint8_t* data);

private:
    uint8_t* const buffer;
    std::size_t head;
    std::size_t tail;
    std::size_t curr;
    const std::size_t max_len;
};
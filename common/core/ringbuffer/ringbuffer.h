/**
 * @file RingBuffer.h
 * @brief Ring Buffer Interface
 */

#pragma once
#include <cstddef>
#include <cstdint>


namespace LBR
{
class RingBuffer
{
public:
    explicit RingBuffer(uint8_t* const buffer_, const size_t max_len_);

    virtual int push(uint8_t data);
    virtual int pop(uint8_t* data);

private:
    uint8_t* const buffer;
    size_t head;
    size_t tail;
    size_t curr;
    const size_t max_len;
};
}  // namespace LBR
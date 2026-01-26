#include "ring_buffer.h"

namespace LBR
{
template <size_t N>
bool RingBuffer<N>::push(uint8_t tx, WritePolicy policy)
{
    // Check if the ring buffer is full
    if (this->full())
    {
        // Check which write policy was picked
        if (policy == WritePolicy::NO_OVERWRITE)
        {
            return false;
        }

        // If we want to overwrite, increment the read pointer
        read = (read + 1) % N;
    }
    else
    {
        count++;
    }

    this->at(write) = tx;
    write = (write + 1) % N;

    return true;
}

// TODO: Impelement pop
template <size_t N>
bool RingBuffer<N>::pop(uint8_t& rx)
{
}

template <size_t N>
bool RingBuffer<N>::peak(uint8_t& rx)
{
    if (!this->empty())
    {
        rx = this->at(read);
        return true;
    }
    return false;
}

template <size_t N>
bool RingBuffer<N>::reset()
{
    if (!this->empty())
    {
        read = 0;
        write = 0;
        count = 0;
        return true;
    }
    return false;
}

template <size_t N>
bool RingBuffer<N>::empty() const
{
    return count == 0;
}

template <size_t N>
bool RingBuffer<N>::full() const
{
    return count == N;
}

template <size_t N>
size_t RingBuffer<N>::size() const
{
    return count;
}

template <size_t N>
size_t RingBuffer<N>::capacity() const
{
    return N;
}
}  // namespace LBR
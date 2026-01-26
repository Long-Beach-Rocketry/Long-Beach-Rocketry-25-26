#include "ring_buffer.h"

namespace LBR
{
template <size_t N>
bool RingBuffer<N>::empty() const
{
    return buf_size == 0;
}

template <size_t N>
bool RingBuffer<N>::full() const
{
    return buf_size == N;
}

template <size_t N>
size_t RingBuffer<N>::size() const
{
    return buf_size;
}

template <size_t N>
size_t RingBuffer<N>::capacity() const
{
    return N;
}
}  // namespace LBR
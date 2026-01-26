/**
 * @file ring_buffer.h
 * @author Kent Hong
 * @brief Ring Buffer API
 * 
 */
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace LBR
{
template <size_t N>
class RingBuffer : private std::array<uint8_t, N>
{
public:
    RingBuffer() = default;

    enum class WritePolicy : uint8_t
    {
        NO_OVERWRITE = 0,
        OVERWRITE
    };

    /**
     * @brief Write a byte of data and advance write indice
     * 
     * @param data Byte of data
     * @param WritePolicy If ring buffer full, choose to overwrite oldest unread data or deny an overwrite on unread data. Default no overwrite.
     * @return True Data was successfully added to the buffer. False otherwise.
     */
    bool push(uint8_t tx, WritePolicy policy = WritePolicy::NO_OVERWRITE);

    /**
     * @brief Read data and advance read indice
     * 
     * @return uint8_t& The data that was just removed from the ring buffer
     */
    bool pop(uint8_t& rx);

    /**
     * @brief Read data without advancing read indice
     * 
     * @return uint8_t& The data that the read index is currently on
     */
    bool peak(uint8_t& rx);

    /**
     * @brief Clear the contents of the ring buffer
     * 
     * @return True Ring Buffer was successfully emptied. False otherwise.
     */
    bool reset();

    /**
     * @brief Checks if the ring buffer is empty
     * 
     * @return True Ring Buffer is empty. False otherwise.
     */
    bool empty() const;

    /**
     * @brief Checks if the ring buffer is full
     * 
     * @return True Ring Buffer is full. False otherwise.
     */
    bool full() const;

    /**
     * @brief Get the current number of elements in the ring buffer
     * 
     * @return size_t Number of elements in the ring buffer
     */
    size_t size() const;

    /**
     * @brief Get the ring buffer's maximum capacity
     * 
     * @return size_t 
     */
    size_t capacity() const;

private:
    size_t count = 0;
    size_t read = 0;
    size_t write = 0;
};
}  // namespace LBR
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
template <typename T, size_t N>
class RingBuffer : private std::array<T, N>
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
    bool push(T tx, WritePolicy policy = WritePolicy::NO_OVERWRITE)
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
            read_index = (read_index + 1) % N;
        }
        else
        {
            count++;
        }

        (*this)[write_index] = tx;
        write_index = (write_index + 1) % N;

        return true;
    }

    /**
     * @brief Read data and advance read indice
     * 
     * @return True Data was successfully read and removed from the buffer. False otherwise.
     */
    bool pop(T& rx)
    {
        if (this->empty())
        {
            return false;
        }

        rx = (*this)[read_index];
        read_index = (read_index + 1) % N;
        count--;

        return true;
    }

    /**
     * @brief Clear the contents of the ring buffer
     * 
     * @return True Ring Buffer was successfully emptied. False otherwise.
     */
    bool reset()
    {
        read_index = 0;
        write_index = 0;
        count = 0;
        return true;
    }

    /**
     * @brief Checks if the ring buffer is empty
     * 
     * @return True Ring Buffer is empty. False otherwise.
     */
    bool empty() const
    {
        return count == 0;
    }

    /**
     * @brief Checks if the ring buffer is full
     * 
     * @return True Ring Buffer is full. False otherwise.
     */
    bool full() const
    {
        return count == N;
    }

    /**
     * @brief Get the current number of elements in the ring buffer
     * 
     * @return size_t Number of elements in the ring buffer
     */
    size_t size() const
    {
        return count;
    }

    /**
     * @brief Get the ring buffer's maximum capacity
     * 
     * @return size_t 
     */
    size_t capacity() const
    {
        return N;
    }

    /**
     * @brief Get the current read index (for testing)
     * 
     * @return size_t 
     */
    size_t get_read() const
    {
        return read_index;
    }

    /**
     * @brief Get the current write index (for testing)
     * 
     * @return size_t 
     */
    size_t get_write() const
    {
        return write_index;
    }

private:
    size_t count = 0;
    size_t read_index = 0;
    size_t write_index = 0;
};
}  // namespace LBR
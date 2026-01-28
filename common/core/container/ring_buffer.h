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
    bool push(uint8_t tx, WritePolicy policy = WritePolicy::NO_OVERWRITE)
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

        (*this)[write] = tx;
        write = (write + 1) % N;

        return true;
    }

    /**
     * @brief Read data and advance read indice
     * 
     * @return uint8_t& The data that was just removed from the ring buffer
     */
    bool pop(uint8_t& rx)
    {
        if (this->empty())
        {
            return false;
        }

        rx = (*this)[read];
        read = (read + 1) % N;
        count--;

        return true;
    }

    /**
     * @brief Read data without advancing read indice
     * 
     * @return uint8_t& The data that the read index is currently on
     */
    bool peek(uint8_t& rx)
    {
        if (!this->empty())
        {
            rx = (*this)[read];
            return true;
        }
        return false;
    }

    /**
     * @brief Clear the contents of the ring buffer
     * 
     * @return True Ring Buffer was successfully emptied. False otherwise.
     */
    bool reset()
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
        return read;
    }

    /**
     * @brief Get the current write index (for testing)
     * 
     * @return size_t 
     */
    size_t get_write() const
    {
        return write;
    }

private:
    size_t count = 0;
    size_t read = 0;
    size_t write = 0;
};
}  // namespace LBR
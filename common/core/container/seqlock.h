/**
 * @file   seqlock.h
 * @author Alex Pulido
 * @brief  Single-writer, multi-reader sequence lock for sharing the latest value
 * @details This lock allows a single writer to update a value while multiple readers can safely read it without blocking,
 *          ensuring that readers either see the complete old value or the complete new value,
 *          but never a partially updated value. Effectively imitating a thread-like lock-free behavior for readers.
 */
#pragma once

#include <atomic>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace LBR
{

/**
 * @brief The writer never blocks; readers retry if they overlap a write and never see a torn value.
 * Only one thread/ISR may call write(). Any number of readers may call read().
 */
template <typename T>
class SeqLock
{
    static_assert(std::is_trivially_copyable_v<T>,
                  "SeqLock requires a trivially copyable type");

public:
    /**
     * @brief Publish a new value
     * @param value Value to publish
     */
    void write(const T& value)
    {
        const uint32_t s = seq.load(std::memory_order_relaxed);
        seq.store(s + 1, std::memory_order_relaxed);  // Odd: write in progress
        std::atomic_thread_fence(std::memory_order_release);
        std::memcpy(&data, &value, sizeof(T));
        seq.store(s + 2, std::memory_order_release);  // Even: write complete
    }

    /**
     * @brief Copy out the latest value
     * @param out Filled with a consistent snapshot on success, untouched on failure
     * @param max_tries Attempts before giving up (8 by default), so reader that preempts the writer cannot spin forever
     * @return True if a consistent snapshot was read, false if no value was published or every attempt overlapped a write
     */
    bool read(T& out, uint32_t max_tries = 8) const
    {
        for (uint32_t i = 0; i < max_tries; i++)
        {
            // Load the sequence number before reading the data
            const uint32_t before = seq.load(std::memory_order_acquire);
            // If the sequence number is zero, no value has been published yet
            if (before == 0)
            {
                return false;
            }
            // If the sequence number is odd, a write is in progress, so retry
            if (before & 1)
            {
                continue;
            }

            T copy;
            std::memcpy(&copy, &data, sizeof(T));
            std::atomic_thread_fence(std::memory_order_acquire);

            // If the sequence number hasn't changed, the read was consistent
            if (seq.load(std::memory_order_relaxed) == before)
            {
                out = copy;
                return true;
            }
        }
        return false;
    }

private:
    std::atomic<uint32_t> seq{0};
    T data{};
};

}  // namespace LBR

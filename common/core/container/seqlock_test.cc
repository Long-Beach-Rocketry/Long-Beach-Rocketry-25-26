/**
 * @file   seqlock_test.cc
 * @author Alex Pulido
 * @brief  Unit tests for single-writer, multi-reader SeqLock
 */

#include "seqlock.h"
#include <gtest/gtest.h>

#include <array>
#include <atomic>
#include <cstdint>

namespace LBR
{
namespace Test
{

/**
 * @brief Sample telemetry data structure for testing SeqLock with trivially copyable types
 */
struct TelemetryData
{
    uint32_t timestamp_ms;
    float altitude;
    float velocity;
    float acceleration;
    bool parachute_deployed;

    bool operator==(const TelemetryData& other) const = default;
};

/**
 * @brief Sample large payload struct for testing multi-word copying
 */
struct LargePayload
{
    uint64_t sequence;
    std::array<uint32_t, 32> buffer;

    bool operator==(const LargePayload& other) const = default;
};

/**
 * @brief Multi-word data structure with an invariant for concurrency validation
 */
struct ConsistentSnapshot
{
    uint64_t sequence;
    uint64_t val1;
    uint64_t val2;
    uint64_t xor_sum;
    std::array<uint32_t, 16> values;
};

static_assert(std::is_trivially_copyable_v<TelemetryData>,
              "TelemetryData must be trivially copyable");
static_assert(std::is_trivially_copyable_v<LargePayload>,
              "LargePayload must be trivially copyable");
static_assert(std::is_trivially_copyable_v<ConsistentSnapshot>,
              "ConsistentSnapshot must be trivially copyable");

/**
 * @brief True when every field of a snapshot matches the published invariant
 */
static bool IsConsistent(const ConsistentSnapshot& snapshot)
{
    // Check the invariant for val1, val2, and xor_sum first
    if (snapshot.xor_sum != (snapshot.val1 ^ snapshot.val2) ||
        snapshot.val2 != snapshot.val1 * 2)
    {
        return false;
    }

    for (uint32_t value : snapshot.values)
    {
        if (value != static_cast<uint32_t>(snapshot.sequence))
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Build a snapshot whose fields satisfy IsConsistent
 */
static ConsistentSnapshot MakeSnapshot(uint64_t sequence)
{
    ConsistentSnapshot data{};
    data.sequence = sequence;
    data.val1 = sequence;
    data.val2 = sequence * 2;
    data.xor_sum = data.val1 ^ data.val2;
    for (uint32_t& value : data.values)
    {
        value = static_cast<uint32_t>(sequence);
    }
    return data;
}

/**
 * @brief Read until done, counting successful and torn snapshots
 */
static void ReadUntilDone(const SeqLock<ConsistentSnapshot>& lock,
                          const std::atomic<bool>& done,
                          std::atomic<uint64_t>& total_successful_reads,
                          std::atomic<uint64_t>& torn_read_count)
{
    while (!done.load(std::memory_order_relaxed))
    {
        ConsistentSnapshot snapshot{};
        if (!lock.read(snapshot))
        {
            continue;
        }

        total_successful_reads.fetch_add(1, std::memory_order_relaxed);
        if (!IsConsistent(snapshot))
        {
            torn_read_count.fetch_add(1, std::memory_order_relaxed);
        }
    }
}

class SeqLockTest : public ::testing::Test
{
protected:
    SeqLockTest() = default;
};

/**
 * @brief Verify that reading from an uninitialized SeqLock returns false and does not alter out.
 */
TEST_F(SeqLockTest, UninitializedReadFailsAndPreservesOut)
{
    SeqLock<uint32_t> lock;
    uint32_t out = 0xDEADBEEFU;

    // Read on uninitialized lock must fail and leave output intact
    EXPECT_FALSE(lock.read(out));
    EXPECT_EQ(out, 0xDEADBEEFU);

    // Should also return false with explicit max_tries
    EXPECT_FALSE(lock.read(out, 1));
    EXPECT_EQ(out, 0xDEADBEEFU);

    EXPECT_FALSE(lock.read(out, 16));
    EXPECT_EQ(out, 0xDEADBEEFU);
}

/**
 * @brief Verify that a const reference to SeqLock can be read.
 */
TEST_F(SeqLockTest, ConstReferenceRead)
{
    SeqLock<int32_t> lock;
    lock.write(1234);

    const SeqLock<int32_t>& const_ref = lock;
    int32_t out = 0;
    EXPECT_TRUE(const_ref.read(out));
    EXPECT_EQ(out, 1234);
}

/**
 * @brief Verify basic write and read operations for primitive types.
 */
TEST_F(SeqLockTest, SingleWriteAndReadPrimitive)
{
    SeqLock<int32_t> lock;
    int32_t out = 0;

    lock.write(42);
    EXPECT_TRUE(lock.read(out));
    EXPECT_EQ(out, 42);

    // Reading again without a new write must still succeed with the same value
    out = 0;
    EXPECT_TRUE(lock.read(out));
    EXPECT_EQ(out, 42);
}

/**
 * @brief Verify that multiple sequential writes update the published value.
 */
TEST_F(SeqLockTest, MultipleSequentialWrites)
{
    SeqLock<uint64_t> lock;
    uint64_t out = 0;

    // Sequentially write values from 1 to 100 and verify each read returns the correct value
    for (uint64_t i = 1; i <= 100; ++i)
    {
        lock.write(i);
        EXPECT_TRUE(lock.read(out));
        EXPECT_EQ(out, i);
    }
}

/**
 * @brief Verify write and read operations with composite trivially copyable structs.
 */
TEST_F(SeqLockTest, TriviallyCopyableStruct)
{
    SeqLock<TelemetryData> lock;
    const TelemetryData initial_out{
        .timestamp_ms = 0,
        .altitude = 0.0f,
        .velocity = 0.0f,
        .acceleration = 0.0f,
        .parachute_deployed = false,
    };
    TelemetryData out = initial_out;

    EXPECT_FALSE(lock.read(out));
    EXPECT_EQ(out, initial_out);

    const TelemetryData sample1{
        .timestamp_ms = 1000,
        .altitude = 125.5f,
        .velocity = 32.1f,
        .acceleration = 9.81f,
        .parachute_deployed = false,
    };
    lock.write(sample1);
    EXPECT_TRUE(lock.read(out));
    EXPECT_EQ(out, sample1);

    const TelemetryData sample2{
        .timestamp_ms = 2500,
        .altitude = 1500.0f,
        .velocity = 250.0f,
        .acceleration = -4.5f,
        .parachute_deployed = true,
    };
    lock.write(sample2);
    EXPECT_TRUE(lock.read(out));
    EXPECT_EQ(out, sample2);
}

/**
 * @brief Verify write and read operations with large data payloads.
 */
TEST_F(SeqLockTest, LargePayloadCopy)
{
    SeqLock<LargePayload> lock;
    LargePayload sample{};
    sample.sequence = 0xFEDCBA9876543210ULL;
    for (size_t i = 0; i < sample.buffer.size(); ++i)
    {
        sample.buffer[i] = static_cast<uint32_t>(i * 101 + 7);
    }

    lock.write(sample);

    LargePayload out{};
    EXPECT_TRUE(lock.read(out));
    EXPECT_EQ(out, sample);
}

/**
 * @brief Verify that max_tries = 0 returns false immediately without altering output.
 */
TEST_F(SeqLockTest, MaxTriesZeroReturnsFalse)
{
    SeqLock<uint32_t> lock;
    lock.write(777);

    uint32_t out = 999;
    EXPECT_FALSE(lock.read(out, 0));
    EXPECT_EQ(out, 999);
}

/**
 * @brief Verify that out parameter is preserved when read fails.
 */
TEST_F(SeqLockTest, PreserveOutOnFailure)
{
    SeqLock<TelemetryData> lock;
    const TelemetryData sentinel{
        .timestamp_ms = 9999,
        .altitude = -1.0f,
        .velocity = -2.0f,
        .acceleration = -3.0f,
        .parachute_deployed = false,
    };
    TelemetryData out = sentinel;

    // Uninitialized read
    EXPECT_FALSE(lock.read(out));
    EXPECT_EQ(out, sentinel);

    // Max tries zero on uninitialized
    EXPECT_FALSE(lock.read(out, 0));
    EXPECT_EQ(out, sentinel);

    // Write a value and read with max_tries = 0
    TelemetryData written{
        .timestamp_ms = 500,
        .altitude = 10.0f,
        .velocity = 20.0f,
        .acceleration = 30.0f,
        .parachute_deployed = true,
    };
    lock.write(written);

    out = sentinel;
    EXPECT_FALSE(lock.read(out, 0));
    EXPECT_EQ(out, sentinel);
}

/**
 * @brief Verify multi-threaded single-writer, multi-reader concurrent access.
 * Validates that readers never observe torn or inconsistent state under high write load.
 */
TEST_F(SeqLockTest, ConcurrentSingleWriterMultiReaderNoTornReads)
{
    SeqLock<ConsistentSnapshot> lock;
    constexpr uint64_t kNumWrites = 30000;
    constexpr size_t kNumReaders = 4;

    std::atomic<bool> done{false};
    std::atomic<uint64_t> total_successful_reads{0};
    std::atomic<uint64_t> torn_read_count{0};

    // Spawn reader threads
    std::vector<std::thread> readers;
    readers.reserve(kNumReaders);

    for (size_t r = 0; r < kNumReaders; ++r)
    {
        readers.emplace_back(ReadUntilDone, std::cref(lock), std::cref(done),
                             std::ref(total_successful_reads),
                             std::ref(torn_read_count));
    }

    // Single writer loop
    for (uint64_t i = 1; i <= kNumWrites; ++i)
    {
        lock.write(MakeSnapshot(i));
    }

    // Signal readers to finish and join threads
    done.store(true, std::memory_order_release);
    for (auto& reader : readers)
    {
        reader.join();
    }

    // Invariant checks
    EXPECT_EQ(torn_read_count.load(), 0) << "Detected torn/inconsistent reads!";
    EXPECT_GT(total_successful_reads.load(), 0)
        << "Expected readers to capture snapshots";

    // Final read must observe the last written sequence
    ConsistentSnapshot final_snapshot{};
    ASSERT_TRUE(lock.read(final_snapshot));
    EXPECT_EQ(final_snapshot.sequence, kNumWrites);
    EXPECT_EQ(final_snapshot.val1, kNumWrites);
    EXPECT_EQ(final_snapshot.val2, kNumWrites * 2);
    EXPECT_EQ(final_snapshot.xor_sum,
              final_snapshot.val1 ^ final_snapshot.val2);
}

/**
 * @brief Verify support for standard types like float, double, bool, and std::array.
 */
TEST_F(SeqLockTest, VariousTriviallyCopyableTypes)
{
    // float
    {
        SeqLock<float> lock;
        float out = 0.0f;
        lock.write(3.14159f);
        EXPECT_TRUE(lock.read(out));
        EXPECT_FLOAT_EQ(out, 3.14159f);
    }

    // double
    {
        SeqLock<double> lock;
        double out = 0.0;
        lock.write(2.718281828459);
        EXPECT_TRUE(lock.read(out));
        EXPECT_DOUBLE_EQ(out, 2.718281828459);
    }

    // bool
    {
        SeqLock<bool> lock;
        bool out = false;
        lock.write(true);
        EXPECT_TRUE(lock.read(out));
        EXPECT_TRUE(out);
        lock.write(false);
        EXPECT_TRUE(lock.read(out));
        EXPECT_FALSE(out);
    }

    // std::array
    {
        using ArrayType = std::array<int32_t, 4>;
        SeqLock<ArrayType> lock;
        ArrayType out{};
        ArrayType sample{10, 20, 30, 40};
        lock.write(sample);
        EXPECT_TRUE(lock.read(out));
        EXPECT_EQ(out, sample);
    }
}

}  // namespace Test
}  // namespace LBR

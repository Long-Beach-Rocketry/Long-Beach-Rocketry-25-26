#pragma once
#include <cstdint>

namespace LBR::Utils
{

/**
 * @brief Delays execution for a specified number of milliseconds (uses SysTick timebase).
 * @param ms The number of milliseconds to delay.
 */
void DelayMs(uint32_t ms);

/**
 * @brief Returns the current millisecond tick count (since boot).
 */
uint32_t GetMsTicks();

/**
 * @brief Returns a free-running microsecond timestamp from the DWT cycle counter.
 * @note Requires the DWT cycle counter to be enabled (see EnableUsTimer()).
 *       Wraps at (2^32 / SystemCoreClock) seconds — e.g. ~67s @ 64 MHz.
 */
uint32_t GetUs();

/**
 * @brief Enables the DWT cycle counter used by GetUs().
 */
void EnableUsTimer();

}  // namespace LBR::Utils
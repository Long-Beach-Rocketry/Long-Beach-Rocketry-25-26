<<<<<<< HEAD
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

=======
#pragma once
#include <cstdint>

namespace LBR::Utils
{
/**
    * @brief Delays execution for a specified number of milliseconds.
    * @param ms The number of milliseconds to delay.
    */
void DelayMs(uint32_t ms);

/**
    * @brief Delays execution for a specified number of microseconds.
    * @param us The number of microseconds to delay.
    */
void DelayUs(uint32_t us);
>>>>>>> 319d772 (ekf)
}  // namespace LBR::Utils
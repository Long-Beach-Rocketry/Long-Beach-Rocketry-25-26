/**
 * @file sys_clock.h
 * @brief System clock interface.
 * @author TJ Malaska
 * @date 1//5/2026
 *
 */

#pragma once
#include <cstdint>
namespace LBR
{
/**
 * @class Clock
 * @brief Configures system clock as well as anything else in the clock
 *  tree needed from cubeMX presets.
 * @note Not designed change clock configs mid application.
 */
class Clock
{
public:
    virtual uint32_t get_hz() const = 0;
    virtual uint32_t get_timingR() const = 0;
};
}  // namespace LBR
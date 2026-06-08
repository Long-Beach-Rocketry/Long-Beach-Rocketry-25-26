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
 * @brief Clock frequencies in Hz
 * 
 */
struct ClockFrequencies
{
    double sysclk{0};
    double cpu{0};
    double ahb{0};
    double apb1{0};
    double apb2{0};
    double apb3{0};
    double apb4{0};
};

/**
 * @class Clock
 * @brief Configures system clock as well as anything else in the clock
 *  tree needed from cubeMX presets.
 * @note Not designed change clock configs mid application.
 */
class Clock
{
public:
    /**
     * @brief Get the frequencies of all clocks in the clock tree (sysclk, d1cpre (cpu), ahb, apb1, apb2, apb3, apb4) in Hz
     * 
     * @return const ClockFrequencies& 
     */
    virtual const ClockFrequencies& get_clock_frequencies() const = 0;
};
}  // namespace LBR
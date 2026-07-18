/**
* @file timebase.h
* @brief Timebase driver interface
* @author Bex Saw
*/

#pragma once

#include <chrono>
#include <cstdint>

namespace LBR
{
class Timebase
{
public:
    /**
    * @brief init the timebase with a specified tick period
    */
    virtual bool init() = 0;

    /**
    * @brief start the timebase
    */
    virtual bool start() = 0;

    /**
    * @brief stop the timebase
    */
    virtual void stop() = 0;

    /**
    * @brief get the current time in microseconds
    * @return The current time in microseconds
    */
    virtual uint64_t uptime_us() const = 0;

    /**
    * @brief Calculate the elapsed time since a previously captured uptime value.
    * @param since_us A timestamp previously returned by uptime_us().
    * @return The elapsed duration in microseconds.
    */
    virtual uint64_t elapsed_since_us(uint64_t since_us) const = 0;

    /**
     * @brief Set the Timer Frequency
     * 
     * @param timer_freq The desired new timer frequency
     * @return true Timer Frequency set successfully, false otherwise
     * @note The reason I expose the pclk & new_timer_freq it's b/c assume 
     * different TIM you want different resolution it is possible to have this as a helper
     */
    virtual bool set_freq(uint32_t new_timer_freq, uint32_t pclk) = 0;

    /**
    * @brief Set the period of the desired timebase TIM channel
    * @param period The desired period in microseconds
    * @return true if the period was set successfully, false otherwise
    */
    virtual bool set_period(std::chrono::microseconds period) = 0;

    virtual ~Timebase() = default;
};
}  // namespace LBR

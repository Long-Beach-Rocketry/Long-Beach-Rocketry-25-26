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
    * @brief get the raw elapsed counter ticks (not converted to microseconds)
    * @return Total elapsed counter ticks since the timebase started.
    * @note Unlike uptime_us(), this value scales with the configured counter
    * frequency, so changing the frequency changes how fast it advances. Useful
    * for observing frequency changes directly.
    */
    virtual uint64_t uptime_ticks() const = 0;

    /**
    * @brief Calculate the elapsed time since a previously captured uptime value.
    * @param since_us A timestamp previously returned by uptime_us().
    * @return The elapsed duration in microseconds.
    */
    virtual uint64_t elapsed_since_us(uint64_t since_us) const = 0;

    /**
     * @brief Set the counter (tick) frequency of the timebase.
     *
     * @param new_counter_freq The desired counter frequency in Hz. The timer's
     * input clock is fixed when the timebase is constructed, so it is not passed
     * in here; the implementation validates the request against that clock.
     * @return true if the counter frequency was set successfully, false otherwise
     */
    virtual bool set_freq(uint32_t new_counter_freq) = 0;

    /**
    * @brief Set the period of the desired timebase TIM channel
    * @param period The desired period in microseconds
    * @return true if the period was set successfully, false otherwise
    */
    virtual bool set_period(std::chrono::microseconds period) = 0;

    virtual ~Timebase() = default;
};
}  // namespace LBR

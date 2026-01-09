/**
 * @file st_sys_clock.h
 * @brief Clock specfics for the stml4.
 * @author TJ Malaska
 * @date
 */

#pragma once

#include <cstdint>
#include "stm32l476xx.h"
#include "sys_clock.h"

namespace LBR::Stml4
{

class HwClock : public Clock
{
public:
    enum clock_src
    {
        HSI16,
        MSI,
        HSE,  // 8 Mhz, same as nucleo
        //PLL   // delete? or is there more? change later
    };

    /**
     * @brief 
     * @return Returns true if success.
     */
    HwClock(uint8_t source);

    /**
     * @brief 
     * @return Returns true if success.
     */
    bool init_clock();

    bool init_pll();

private:
    /**
     * only needed if clock speed is above 24mhz
     */
    bool init_flash_latency();

    /* configs */
    uint8_t source_;
    uint8_t prediv;
    uint8_t pllmul;
};

}  // namespace LBR::Stml4
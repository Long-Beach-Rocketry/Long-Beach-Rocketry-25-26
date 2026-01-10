/**
 * @file pps_hw.h
 * @brief Hardware abstraction for PPS limit switch (PA3)
 */

#pragma once
#include "stm32l476xx.h"

namespace LBR {
    /**
     * @brief Read the PPS limit switch (PA3)
     * @return true if switch is active, false otherwise
     */
    bool lms();
}




/**
 * @file st_sysclk.h
 * @author Kent Hong
 * @brief Clock configurations for the STM32H7
 */

#pragma once

#include <cstdint>
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "sys_clock.h"

namespace LBR
{
namespace Stmh7
{

enum class Source
{
    HSI64_MHZ,
    HSE8_MHZ
};

/**
     * @brief Prescales Sysclk to acquire D1CPRE Clock
     * @example If Sysclk is 550 MHz and SysclkPrescaler is DIV2, then Hclk is 275 MHz (550 MHz / 2 = 275 MHz)
     * @note Max frequency after prescale is 550 MHz
     */
enum class D1cprePrescaler : uint32_t
{
    DIV1 = RCC_SYSCLK_DIV1,
    DIV2 = RCC_SYSCLK_DIV2,
    DIV4 = RCC_SYSCLK_DIV4,
    DIV8 = RCC_SYSCLK_DIV8,
    DIV16 = RCC_SYSCLK_DIV16,
    DIV64 = RCC_SYSCLK_DIV64,
    DIV128 = RCC_SYSCLK_DIV128,
    DIV256 = RCC_SYSCLK_DIV256,
    DIV512 = RCC_SYSCLK_DIV512
};

/**
     * @brief Prescales D1CPRE Clock to acquire all AHB clocks (HCLK)
     * @note Max frequency after prescale is 275 MHz
     */
enum class AhbPrescaler : uint32_t
{
    DIV1 = RCC_HCLK_DIV1,
    DIV2 = RCC_HCLK_DIV2,
    DIV4 = RCC_HCLK_DIV4,
    DIV8 = RCC_HCLK_DIV8,
    DIV16 = RCC_HCLK_DIV16,
    DIV64 = RCC_HCLK_DIV64,
    DIV128 = RCC_HCLK_DIV128,
    DIV256 = RCC_HCLK_DIV256,
    DIV512 = RCC_HCLK_DIV512
};

/**
     * @brief Prescales AHB clock to acquire APB1 clock (PCLK1)
     * @note Max frequency after prescale is 137.5 MHz
     */
enum class Apb1Prescaler : uint32_t
{
    DIV1 = RCC_APB1_DIV1,
    DIV2 = RCC_APB1_DIV2,
    DIV4 = RCC_APB1_DIV4,
    DIV8 = RCC_APB1_DIV8,
    DIV16 = RCC_APB1_DIV16
};

/**
     * @brief Prescales AHB clock to acquire APB2 clock (PCLK2)
     * @note Max frequency after prescale is 137.5 MHz
     */
enum class Apb2Prescaler : uint32_t
{
    DIV1 = RCC_APB2_DIV1,
    DIV2 = RCC_APB2_DIV2,
    DIV4 = RCC_APB2_DIV4,
    DIV8 = RCC_APB2_DIV8,
    DIV16 = RCC_APB2_DIV16
};

/**
     * @brief Prescales AHB clock to acquire APB3 clock (PCLK3)
     * @note Max frequency after prescale is 137.5 MHz
     */
enum class Apb3Prescaler : uint32_t
{
    DIV1 = RCC_APB3_DIV1,
    DIV2 = RCC_APB3_DIV2,
    DIV4 = RCC_APB3_DIV4,
    DIV8 = RCC_APB3_DIV8,
    DIV16 = RCC_APB3_DIV16
};

/**
     * @brief Prescales AHB clock to acquire APB4 clock (PCLK4)
     * @note Max frequency after prescale is 137.5 MHz
     */
enum class Apb4Prescaler : uint32_t
{
    DIV1 = RCC_APB4_DIV1,
    DIV2 = RCC_APB4_DIV2,
    DIV4 = RCC_APB4_DIV4,
    DIV8 = RCC_APB4_DIV8,
    DIV16 = RCC_APB4_DIV16
};

struct ClockParams
{
    Source clk_src;
    uint32_t sysclk;             // Max 550 MHz
    D1cprePrescaler d1cpre_psc;  // Max 550 MHz
    AhbPrescaler ahb_psc;        // Max 275 MHz
    Apb1Prescaler apb1_psc;      // Max 137.5 MHz
    Apb2Prescaler apb2_psc;      // Max 137.5 MHz
    Apb3Prescaler apb3_psc;      // Max 137.5 MHz
    Apb4Prescaler apb4_psc;      // Max 137.5 MHz
};

/**
 * @class HwClock
 * @brief Chooses clock source (HSI64 or HSE) and configures system clock, hclk (AHB), pclk1 (APB1), and pclk2 (APB2).
 * @note Only supports HSI RC and HSE for now and cannot change clock configs during runtime.
 * 
 */
class HwClock : public Clock
{
public:
    explicit HwClock(const ClockParams& params_);

    /**
         * @brief Initializes clock tree based on selected clock source (HSI64 or HSE8) and desired sysclk frequency (max 550 MHz)
         * 
         * @return true Clock tree init success, false otherwise
         */
    bool init();

    /**
         * @brief Get all clock frequencies (sysclk, d1cpre, ahb, apb1, apb2, apb3, apb4) in Hz
         * 
         * @return ClockFrequencies struct of all current clock frequencies (sysclk, d1cpre, ahb, apb1, apb2, apb3, apb4)
         */
    const ClockFrequencies& get_clock_frequencies() const override;

private:
    bool validate_params();

    void set_frequencies();

    bool SystemClock_ConfigHSI64();

    bool SystemClock_ConfigHSE8();

    bool calc_sysclk_vars(uint32_t& divn, uint32_t& fracn, uint32_t f_ref_ck);

    ClockParams params;
    ClockFrequencies frequencies;
};
}  // namespace Stmh7
}  // namespace LBR

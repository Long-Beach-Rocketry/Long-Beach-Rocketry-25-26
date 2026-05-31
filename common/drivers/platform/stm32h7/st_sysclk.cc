#include "st_sysclk.h"

namespace
{
static constexpr uint32_t kMaxSysclk{550'000'000};
static constexpr uint32_t kMaxD1cpre{550'000'000};
static constexpr uint32_t kMaxAhb{275'000'000};
static constexpr uint32_t kMaxApb{137'500'000};

struct ConvertedPrescalers
{
    uint32_t d1cpre_psc;
    uint32_t ahb_psc;
    uint32_t apb1_psc;
    uint32_t apb2_psc;
    uint32_t apb3_psc;
    uint32_t apb4_psc;
};

static inline ConvertedPrescalers convert_prescalers(
    LBR::Stmh7::D1cprePrescaler d1cpre_psc, LBR::Stmh7::AhbPrescaler ahb_psc,
    LBR::Stmh7::Apb1Prescaler apb1_psc, LBR::Stmh7::Apb2Prescaler apb2_psc,
    LBR::Stmh7::Apb3Prescaler apb3_psc, LBR::Stmh7::Apb4Prescaler apb4_psc)
{
    ConvertedPrescalers prescalers;

    switch (d1cpre_psc)
    {
        case LBR::Stmh7::D1cprePrescaler::DIV1:
            prescalers.d1cpre_psc = 1;
            break;
        case LBR::Stmh7::D1cprePrescaler::DIV2:
            prescalers.d1cpre_psc = 2;
            break;
        case LBR::Stmh7::D1cprePrescaler::DIV4:
            prescalers.d1cpre_psc = 4;
            break;
        case LBR::Stmh7::D1cprePrescaler::DIV8:
            prescalers.d1cpre_psc = 8;
            break;
        case LBR::Stmh7::D1cprePrescaler::DIV16:
            prescalers.d1cpre_psc = 16;
            break;
        case LBR::Stmh7::D1cprePrescaler::DIV64:
            prescalers.d1cpre_psc = 64;
            break;
        case LBR::Stmh7::D1cprePrescaler::DIV128:
            prescalers.d1cpre_psc = 128;
            break;
        case LBR::Stmh7::D1cprePrescaler::DIV256:
            prescalers.d1cpre_psc = 256;
            break;
        case LBR::Stmh7::D1cprePrescaler::DIV512:
            prescalers.d1cpre_psc = 512;
    }

    switch (ahb_psc)
    {
        case LBR::Stmh7::AhbPrescaler::DIV1:
            prescalers.ahb_psc = 1;
            break;
        case LBR::Stmh7::AhbPrescaler::DIV2:
            prescalers.ahb_psc = 2;
            break;
        case LBR::Stmh7::AhbPrescaler::DIV4:
            prescalers.ahb_psc = 4;
            break;
        case LBR::Stmh7::AhbPrescaler::DIV8:
            prescalers.ahb_psc = 8;
            break;
        case LBR::Stmh7::AhbPrescaler::DIV16:
            prescalers.ahb_psc = 16;
            break;
        case LBR::Stmh7::AhbPrescaler::DIV64:
            prescalers.ahb_psc = 64;
            break;
        case LBR::Stmh7::AhbPrescaler::DIV128:
            prescalers.ahb_psc = 128;
            break;
        case LBR::Stmh7::AhbPrescaler::DIV256:
            prescalers.ahb_psc = 256;
            break;
        case LBR::Stmh7::AhbPrescaler::DIV512:
            prescalers.ahb_psc = 512;
    }

    switch (apb1_psc)
    {
        case LBR::Stmh7::Apb1Prescaler::DIV1:
            prescalers.apb1_psc = 1;
            break;
        case LBR::Stmh7::Apb1Prescaler::DIV2:
            prescalers.apb1_psc = 2;
            break;
        case LBR::Stmh7::Apb1Prescaler::DIV4:
            prescalers.apb1_psc = 4;
            break;
        case LBR::Stmh7::Apb1Prescaler::DIV8:
            prescalers.apb1_psc = 8;
            break;
        case LBR::Stmh7::Apb1Prescaler::DIV16:
            prescalers.apb1_psc = 16;
    }

    switch (apb2_psc)
    {
        case LBR::Stmh7::Apb2Prescaler::DIV1:
            prescalers.apb2_psc = 1;
            break;
        case LBR::Stmh7::Apb2Prescaler::DIV2:
            prescalers.apb2_psc = 2;
            break;
        case LBR::Stmh7::Apb2Prescaler::DIV4:
            prescalers.apb2_psc = 4;
            break;
        case LBR::Stmh7::Apb2Prescaler::DIV8:
            prescalers.apb2_psc = 8;
            break;
        case LBR::Stmh7::Apb2Prescaler::DIV16:
            prescalers.apb2_psc = 16;
    }

    switch (apb3_psc)
    {
        case LBR::Stmh7::Apb3Prescaler::DIV1:
            prescalers.apb3_psc = 1;
            break;
        case LBR::Stmh7::Apb3Prescaler::DIV2:
            prescalers.apb3_psc = 2;
            break;
        case LBR::Stmh7::Apb3Prescaler::DIV4:
            prescalers.apb3_psc = 4;
            break;
        case LBR::Stmh7::Apb3Prescaler::DIV8:
            prescalers.apb3_psc = 8;
            break;
        case LBR::Stmh7::Apb3Prescaler::DIV16:
            prescalers.apb3_psc = 16;
    }

    switch (apb4_psc)
    {
        case LBR::Stmh7::Apb4Prescaler::DIV1:
            prescalers.apb4_psc = 1;
            break;
        case LBR::Stmh7::Apb4Prescaler::DIV2:
            prescalers.apb4_psc = 2;
            break;
        case LBR::Stmh7::Apb4Prescaler::DIV4:
            prescalers.apb4_psc = 4;
            break;
        case LBR::Stmh7::Apb4Prescaler::DIV8:
            prescalers.apb4_psc = 8;
            break;
        case LBR::Stmh7::Apb4Prescaler::DIV16:
            prescalers.apb4_psc = 16;
    }

    return prescalers;
}

static inline bool validate_params(const LBR::Stmh7::ClockParams& params,
                                   LBR::Stmh7::ClockFrequencies& frequencies)
{
    ConvertedPrescalers prescalers =
        convert_prescalers(params.d1cpre_psc, params.ahb_psc, params.apb1_psc,
                           params.apb2_psc, params.apb3_psc, params.apb4_psc);

    uint32_t d1cpre_clk = params.sysclk / prescalers.d1cpre_psc;
    uint32_t ahb_clk = d1cpre_clk / prescalers.ahb_psc;
    uint32_t apb1_clk = ahb_clk / prescalers.apb1_psc;
    uint32_t apb2_clk = ahb_clk / prescalers.apb2_psc;
    uint32_t apb3_clk = ahb_clk / prescalers.apb3_psc;
    uint32_t apb4_clk = ahb_clk / prescalers.apb4_psc;

    if ((params.sysclk <= kMaxSysclk) && (d1cpre_clk <= kMaxD1cpre) &&
        (ahb_clk <= kMaxAhb) && (apb1_clk <= kMaxApb) &&
        (apb2_clk <= kMaxApb) && (apb3_clk <= kMaxApb) && (apb4_clk <= kMaxApb))
    {
        frequencies.sysclk = params.sysclk;
        frequencies.d1cpre = d1cpre_clk;
        frequencies.ahb = ahb_clk;
        frequencies.apb1 = apb1_clk;
        frequencies.apb2 = apb2_clk;
        frequencies.apb3 = apb3_clk;
        frequencies.apb4 = apb4_clk;
        return true;
    }

    return false;
}
}  // namespace

namespace LBR
{
namespace Stmh7
{

HwClock::HwClock(const ClockParams& params_) : params(params_)
{
}

bool HwClock::init()
{
    if (!validate_params(params, frequencies))
    {
        return false;
    }

    HAL_Init();

    switch (params.clk_src)
    {
        case Source::HSI64_MHZ:
            return SystemClock_ConfigHSI64();
        case Source::HSE8_MHZ:
            return SystemClock_ConfigHSE8();
        default:
            return false;
    }
}

const ClockFrequencies& HwClock::get_clock_frequencies() const
{
    return frequencies;
}

// TODO: Implement SystemClock_ConfigHSI64 and SystemClock_ConfigHSE8 based on cubeMX generated code for HSI64 and HSE8 clock sources respectively. These functions should configure the clock tree based on the selected clock source and desired sysclk frequency, and return true if clock tree init is successful and false otherwise.

}  // namespace Stmh7
}  // namespace LBR

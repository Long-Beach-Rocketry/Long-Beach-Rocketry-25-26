#include "st_sysclk.h"

namespace
{
static constexpr uint32_t kMaxSysclk{550'000'000};
static constexpr uint32_t kMaxD1cpre{550'000'000};
static constexpr uint32_t kMaxAhb{275'000'000};
static constexpr uint32_t kMaxApb{137'500'000};
static constexpr uint32_t kHsi64Mhz{64'000'000};
static constexpr uint32_t kHse8Mhz{8'000'000};
static constexpr uint32_t kMinHseFreq{4'000'000};
static constexpr uint32_t kMaxHseFreq{50'000'000};
static constexpr uint32_t kFlashStepHz{70'000'000};
static constexpr uint32_t kVcoThreshold3{200'000'000};
static constexpr uint32_t kVcoThreshold2{300'000'000};
static constexpr uint32_t kVcoThreshold1{400'000'000};
static constexpr uint32_t kMinPllInputHz{1'000'000};
static constexpr uint32_t kMaxPllInputHz{16'000'000};
static constexpr uint32_t kPllMediumVcoMinHz{150'000'000};
static constexpr uint32_t kPllMediumVcoMaxHz{420'000'000};
static constexpr uint32_t kFracDenom{1U << 13};

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

static inline uint32_t select_flash_latency(uint32_t hclk_hz)
{
    // Conservative flash wait-state selection at about 70 MHz per WS.
    if (hclk_hz <= (1U * kFlashStepHz))
    {
        return FLASH_LATENCY_0;
    }
    if (hclk_hz <= (2U * kFlashStepHz))
    {
        return FLASH_LATENCY_1;
    }
    if (hclk_hz <= (3U * kFlashStepHz))
    {
        return FLASH_LATENCY_2;
    }
    if (hclk_hz <= (4U * kFlashStepHz))
    {
        return FLASH_LATENCY_3;
    }
    if (hclk_hz <= (5U * kFlashStepHz))
    {
        return FLASH_LATENCY_4;
    }
    if (hclk_hz <= (6U * kFlashStepHz))
    {
        return FLASH_LATENCY_5;
    }
    if (hclk_hz <= (7U * kFlashStepHz))
    {
        return FLASH_LATENCY_6;
    }
    return FLASH_LATENCY_7;
}

static inline bool select_pll1_vci_range(uint32_t pll_input_hz,
                                         uint32_t& pll1_vci_range)
{
    if ((pll_input_hz < kMinPllInputHz) || (pll_input_hz > kMaxPllInputHz))
    {
        return false;
    }

    if (pll_input_hz < 2'000'000U)
    {
        pll1_vci_range = RCC_PLL1VCIRANGE_0;
    }
    else if (pll_input_hz < 4'000'000U)
    {
        pll1_vci_range = RCC_PLL1VCIRANGE_1;
    }
    else if (pll_input_hz < 8'000'000U)
    {
        pll1_vci_range = RCC_PLL1VCIRANGE_2;
    }
    else
    {
        // Keep boundary behavior consistent with the previous 8 MHz setup.
        pll1_vci_range = RCC_PLL1VCIRANGE_3;
    }

    return true;
}

static inline uint32_t select_pll1_vco_range(uint32_t pll_input_hz,
                                             uint32_t divn, uint32_t fracn)
{
    uint64_t scaled = (static_cast<uint64_t>(divn) * kFracDenom) +
                      static_cast<uint64_t>(fracn);
    uint32_t vco_hz = static_cast<uint32_t>(
        (static_cast<uint64_t>(pll_input_hz) * scaled + (kFracDenom / 2U)) /
        kFracDenom);

    if ((vco_hz >= kPllMediumVcoMinHz) && (vco_hz <= kPllMediumVcoMaxHz))
    {
        return RCC_PLL1VCOMEDIUM;
    }

    return RCC_PLL1VCOWIDE;
}

static inline uint32_t apb_divisor_from_rcc(uint32_t apb_divider)
{
    if ((apb_divider == RCC_APB3_DIV1) || (apb_divider == RCC_APB4_DIV1))
    {
        return 1U;
    }
    if ((apb_divider == RCC_APB3_DIV2) || (apb_divider == RCC_APB4_DIV2))
    {
        return 2U;
    }
    if ((apb_divider == RCC_APB3_DIV4) || (apb_divider == RCC_APB4_DIV4))
    {
        return 4U;
    }
    if ((apb_divider == RCC_APB3_DIV8) || (apb_divider == RCC_APB4_DIV8))
    {
        return 8U;
    }
    if ((apb_divider == RCC_APB3_DIV16) || (apb_divider == RCC_APB4_DIV16))
    {
        return 16U;
    }

    return 1U;
}

static inline void configure_voltage_scaling(uint32_t core_clk)
{
    if (core_clk <= kVcoThreshold3)
    {
        __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
    }
    else if (core_clk <= kVcoThreshold2)
    {
        __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
    }
    else if (core_clk <= kVcoThreshold1)
    {
        __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    }
    else
    {
        __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
    }

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }
}
}  // namespace

namespace LBR
{
namespace Stmh7
{

HwClock::HwClock(const ClockParams& params_) : params(params_)
{
}

bool HwClock::validate_params()
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
        frequencies.cpu = d1cpre_clk;
        frequencies.ahb = ahb_clk;
        frequencies.apb1 = apb1_clk;
        frequencies.apb2 = apb2_clk;
        frequencies.apb3 = apb3_clk;
        frequencies.apb4 = apb4_clk;
        return true;
    }

    return false;
}

bool HwClock::init()
{
    if (!validate_params())
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

bool HwClock::SystemClock_ConfigHSI64()
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    constexpr uint32_t pll_m = 4U;
    uint32_t core_clk = frequencies.cpu;
    uint32_t flash_latency = select_flash_latency(frequencies.ahb);
    uint32_t pll1_vci_range;
    uint32_t pll_input_hz = kHsi64Mhz / pll_m;

    if (!select_pll1_vci_range(pll_input_hz, pll1_vci_range))
    {
        return false;
    }

    /** Supply configuration update enable
  */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

    /** Configure the main internal regulator output voltage
  */
    configure_voltage_scaling(core_clk);

    uint32_t divn, fracn;

    if (!calc_sysclk_vars(divn, fracn, kHsi64Mhz))
    {
        return false;
    }

    uint32_t pll1_vco_range = select_pll1_vco_range(pll_input_hz, divn, fracn);

    /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = 64;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = pll_m;
    RCC_OscInitStruct.PLL.PLLN = divn;
    RCC_OscInitStruct.PLL.PLLP = 1;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = pll1_vci_range;
    RCC_OscInitStruct.PLL.PLLVCOSEL = pll1_vco_range;
    RCC_OscInitStruct.PLL.PLLFRACN = fracn;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        return false;
    }

    /** Initializes the CPU, AHB and APB buses clocks
  */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 |
                                  RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = static_cast<uint32_t>(params.d1cpre_psc);
    RCC_ClkInitStruct.AHBCLKDivider = static_cast<uint32_t>(params.ahb_psc);
    RCC_ClkInitStruct.APB3CLKDivider = static_cast<uint32_t>(params.apb3_psc);
    RCC_ClkInitStruct.APB1CLKDivider = static_cast<uint32_t>(params.apb1_psc);
    RCC_ClkInitStruct.APB2CLKDivider = static_cast<uint32_t>(params.apb2_psc);
    RCC_ClkInitStruct.APB4CLKDivider = static_cast<uint32_t>(params.apb4_psc);

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, flash_latency) != HAL_OK)
    {
        return false;
    }

    set_frequencies();
    return true;
}

bool HwClock::SystemClock_ConfigHSE8()
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    constexpr uint32_t pll_m = 1U;
    uint32_t core_clk = frequencies.cpu;
    uint32_t flash_latency = select_flash_latency(frequencies.ahb);
    uint32_t pll1_vci_range;
    uint32_t pll_input_hz = kHse8Mhz / pll_m;

    if (!select_pll1_vci_range(pll_input_hz, pll1_vci_range))
    {
        return false;
    }

    /** Supply configuration update enable
  */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

    /** Configure the main internal regulator output voltage
  */
    configure_voltage_scaling(core_clk);

    uint32_t divn, fracn;

    if (!calc_sysclk_vars(divn, fracn, kHse8Mhz))
    {
        return false;
    }

    uint32_t pll1_vco_range = select_pll1_vco_range(pll_input_hz, divn, fracn);

    /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = pll_m;
    RCC_OscInitStruct.PLL.PLLN = divn;
    RCC_OscInitStruct.PLL.PLLP = 1;
    RCC_OscInitStruct.PLL.PLLQ = 4;  // Ignore and leave
    RCC_OscInitStruct.PLL.PLLR = 2;  // Ignore and leave
    RCC_OscInitStruct.PLL.PLLRGE = pll1_vci_range;
    RCC_OscInitStruct.PLL.PLLVCOSEL = pll1_vco_range;
    RCC_OscInitStruct.PLL.PLLFRACN = fracn;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        return false;
    }

    /** Initializes the CPU, AHB and APB buses clocks
  */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 |
                                  RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = static_cast<uint32_t>(params.d1cpre_psc);
    RCC_ClkInitStruct.AHBCLKDivider = static_cast<uint32_t>(params.ahb_psc);
    RCC_ClkInitStruct.APB3CLKDivider = static_cast<uint32_t>(params.apb3_psc);
    RCC_ClkInitStruct.APB1CLKDivider = static_cast<uint32_t>(params.apb1_psc);
    RCC_ClkInitStruct.APB2CLKDivider = static_cast<uint32_t>(params.apb2_psc);
    RCC_ClkInitStruct.APB4CLKDivider = static_cast<uint32_t>(params.apb4_psc);

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, flash_latency) != HAL_OK)
    {
        return false;
    }

    set_frequencies();
    return true;
}

bool HwClock::calc_sysclk_vars(uint32_t& divn, uint32_t& fracn,
                               uint32_t f_ref_ck)
{
    static constexpr uint32_t kMinDivn{4U};
    static constexpr uint32_t kMaxDivn{512U};

    if ((f_ref_ck == 0U) || (params.sysclk == 0U) ||
        (params.sysclk > kMaxSysclk))
    {
        return false;
    }

    if ((params.clk_src == Source::HSE8_MHZ) &&
        ((f_ref_ck < kMinHseFreq) || (f_ref_ck > kMaxHseFreq)))
    {
        return false;
    }

    if ((params.clk_src == Source::HSI64_MHZ) && (f_ref_ck != kHsi64Mhz))
    {
        return false;
    }

    // Round to the nearest FRACN step: scaled = DIVN * 2^13 + FRACN.
    uint64_t scaled = ((static_cast<uint64_t>(params.sysclk) * kFracDenom) +
                       (f_ref_ck / 2U)) /
                      static_cast<uint64_t>(f_ref_ck);

    uint32_t divn_tmp = static_cast<uint32_t>(scaled / kFracDenom);
    uint32_t fracn_tmp = static_cast<uint32_t>(scaled % kFracDenom);

    if (divn_tmp < kMinDivn)
    {
        return false;
    }

    if (divn_tmp > kMaxDivn)
    {
        return false;
    }

    divn = divn_tmp;
    fracn = fracn_tmp;
    return true;
}

void HwClock::set_frequencies()
{
    RCC_ClkInitTypeDef clk_config = {0};
    uint32_t flash_latency = 0;

    HAL_RCC_GetClockConfig(&clk_config, &flash_latency);
    SystemCoreClockUpdate();

    frequencies.sysclk = HAL_RCC_GetSysClockFreq();
    frequencies.cpu = SystemCoreClock;
    frequencies.ahb = HAL_RCC_GetHCLKFreq();
    frequencies.apb1 = HAL_RCC_GetPCLK1Freq();
    frequencies.apb2 = HAL_RCC_GetPCLK2Freq();
    frequencies.apb3 =
        frequencies.ahb / apb_divisor_from_rcc(clk_config.APB3CLKDivider);
    frequencies.apb4 =
        frequencies.ahb / apb_divisor_from_rcc(clk_config.APB4CLKDivider);
}
}  // namespace Stmh7
}  // namespace LBR

#include <array>
#include "delay.h"
#include "st_sysclk.h"
#include "thvd1420.h"

int main()
{
    // Setting up the Clock Tree to drive the MCU safely at 275 MHz
    LBR::Stmh7::ClockParams clock_config{
        .clk_src = LBR::Stmh7::Source::HSE8_MHZ_BYPASS,
        .sysclk = 275'000'000,
        .d1cpre_psc = LBR::Stmh7::D1cprePrescaler::DIV1,
        .ahb_psc = LBR::Stmh7::AhbPrescaler::DIV1,    // AHB (HCLK) = 275 MHz
        .apb1_psc = LBR::Stmh7::Apb1Prescaler::DIV2,  // APB1 Bus = 137.5 MHz
        .apb2_psc = LBR::Stmh7::Apb2Prescaler::DIV2,
        .apb3_psc = LBR::Stmh7::Apb3Prescaler::DIV2,
        .apb4_psc = LBR::Stmh7::Apb4Prescaler::DIV2};

    LBR::Stmh7::HwClock clock_manager(clock_config);
    if (!clock_manager.init())
    {
        while (1);
    }

    // Query the live APB1 bus clock frequency powering USART peripheral
    uint32_t active_usart_clock =
        static_cast<uint32_t>(clock_manager.get_clock_frequencies().apb1);

    // Configure and allocate the THVD1420 driver instances statically
    LBR::Stmh7::RS485Params rs485_config{
        .base_addr = USART3,  // Injects specific base hardware pointer
        .baud_rate = 115200   // Standard, stable target test rate
    };

    LBR::Stmh7::Thvd1420 rs485_bus(rs485_config);

    // Initialize the module with dynamic system bus speed
    if (!rs485_bus.init(active_usart_clock))
    {
        while (1);
    }

    std::array<uint8_t, 4> tx_payload = {0xDE, 0xAD, 0xBE, 0xEF};  // 0xDEADBEEF
    uint8_t rx_byte = 0;

    while (1)
    {
        // Constantly check for incoming bytes every loop iteration
        if (rs485_bus.receive(rx_byte))
        {
            // If we receive a byte, echo it back immediately to verify bidirectional data flow
            std::array<uint8_t, 1> echo_byte = {rx_byte};
            rs485_bus.send(echo_byte);
        }

        // Send our "DEADBEEF" packet once every 500 milliseconds
        uint32_t current_time = LBR::Utils::GetMsTicks();
        if ((current_time - last_transmit_time) >= 500)
        {
            rs485_bus.send(tx_payload);
            last_transmit_time = current_time;
        }
    }
}
/**
 * @file ft_app.h
 * @brief Flight telemetry application coordinating data ingestion, storage, and downlink.
 * @author Alex Pulido
 */
#pragma once 

#include <cstdint>
#include <span>
#include "lora.h"
#include "usart_pipe.h"
#include "w25q.h"

namespace LBR
{

// TODO: Placeholder class for when the LoRa driver is done, names/members/params here may need to change
class LoRa 
{
public:
    virtual LoRa() = default;
    virtual bool transmit(std::span<const uint8_t> data) = 0;
};

/**
 * @class FlightTelemApp
 * @brief Coordinates receiving, logging, and downlinking telemetry data.
 * 
 * @details Acts as a transparent bridge on the flight telemetry board:
 *      1. Extracts validated raw frames arriving from the airbrake board via Pipeline.
 *      2. Logs frames sequentially to an onboard W25Q SPI flash memory (black box).
 *      3. Forwards frames directly to the LoRa driver for ground telemetry transmission.
 */
class FlightTelemApp 
{
public:
    // Pass flash as an optional pointer (nullptr disables flash logging)
    FlightTelemApp(Pipeline& pipeline, LoRa& lora, W25q* flash = nullptr);

    /**
     * @brief Prepares underlying peripherals and initial storage regions.
     * @return True if initialization and initial sector erase succeeded, false otherwise.
     */
    bool init();

    /**
     * @brief Main execution routine. Polls and processes incoming telemetry frames.
     * @details Extracts verified 256-byte frames from the Pipeline's internal ring buffer,
     *          commits them to flash memory (if present), and triggers LoRa downlink.
     */
    void update();

private:
    Pipeline& pipeline_;
    LoRa& lora_;
    W25q* flash_;

    // Flash logging memory
    uint8_t cur_block_{0};
    uint8_t cur_sector_{0};
    uint8_t cur_page_{0};

    std::array<uint8_t, 256> frame_buffer_{};
    std::array<uint8_t, 256> flash_verify_buffer_{}; // Required by w25q page_program for verification

    /**
     * @brief Writes a 256-byte frame to the active flash page and advances cursors.
     * @param frame Span containing the 256-byte raw frame.
     */
    void log_to_flash(std::span<uint8_t> frame);
};

} // namespace LBR
/**
 * @file ft_app.cc
 * @brief Flight telemetry application coordinating data ingestion, storage, and downlink.
 * @author Alex Pulido
 */
#include "ft_app.h"

namespace LBR
{

/**
 * @brief Constructs the Flight Telemetry Application instance.
 * @param pipeline Reference to the board-to-board framing Pipeline.
 * @param lora Reference to the LoRa transceiver driver interface.
 * @param flash Optional pointer to the W25Q SPI flash driver (nullptr disables logging).
 */
FlightTelemApp::FlightTelemApp(Pipeline& pipeline, LoRa& lora, W25q* flash)
    : pipeline_(pipeline), lora_(lora), flash_(flash)
{
}

/**
 * @brief Initializes dependent subsystems and prepares flash storage.
 * @return True if initialization and initial sector preparation succeeded; false otherwise.
 * @note If a flash driver is attached, this unlocks blocks and erases the initial sector.
 */
bool FlightTelemApp::init()
{
    if (flash_ != nullptr)
    {
        if (!flash_->init())
        {
            return false;
        }
        
        // Erase the starting sector for clean flight writes
        if (!flash_->sector_erase(cur_block_, cur_sector_))
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Main polling routine to process incoming frames and dispatch them.
 * 
 * @details Checks the internal Pipeline ring buffer for a complete, validated frame
 *          using `Pipeline::receive_raw()`. This routine checks framing markers (SOF/EOF)
 *          and CRC32 validation. If a valid fixed-size 256-byte frame is found, it:
 *          1. Backs up the raw frame into non-volatile SPI flash memory (if enabled).
 *          2. Dispatches the frame to the LoRa driver for RF downlink to ground.
 * 
 * @note Non-blocking with respect to packet extraction; should be called continuously
 *       inside the main application loop.
 */
void FlightTelemApp::update()
{
    uint16_t frame_len = 0;

    // Pull validated raw frame out of the pipeline's RX ring buffer
    if (pipeline_.receive_raw(std::span<uint8_t>(frame_buffer_), frame_len))
    {
        std::span<uint8_t> valid_frame(frame_buffer_.data(), frame_len);

        // Backup to onboard flash storage if detected
        if (flash_ != nullptr)
        {
            log_to_flash(valid_frame);
        }

        // Downlink to ground telemetry over LoRa
        lora_.transmit(valid_frame);
    }
}

/**
 * @brief Writes a telemetry frame to the W25Q flash chip and advances the write cursor.
 * 
 * @param frame Span containing the 256-byte raw frame to commit to memory.
 * 
 * @details Writes the frame to the current page address (block, sector, page, offset 0).
 *          Utilizes `flash_verify_buffer_` to satisfy `W25q::page_program` 's immediate 
 *          read-back verification. Upon successful commit, advances `cur_page_`.
 *          When crossing a 16-page sector boundary (4 KB), it advances `cur_sector_`
 *          and executes `W25q::sector_erase()` on the upcoming sector so it is cleared
 *          before the next write.
 * 
 * @warning `sector_erase()` is a blocking SPI operation while the flash clears bits to 1.
 */
void FlightTelemApp::log_to_flash(std::span<uint8_t> frame)
{
    // Write 256-byte frame into the current page and verify
    flash_->page_program(cur_block_, cur_sector_, cur_page_, 0,
                         frame, std::span<uint8_t>(flash_verify_buffer_));

    // Advance cursor to the next page
    cur_page_++;
    if (cur_page_ >= 16)
    {
        cur_page_ = 0;
        cur_sector_++;

        if (cur_sector_ >= 16)
        {
            cur_sector_ = 0;
            cur_block_++; // Moves to next 64KB block
        }

        // Prepare the new sector before writing to it
        flash_->sector_erase(cur_block_, cur_sector_);
    }
}

}  // namespace LBR
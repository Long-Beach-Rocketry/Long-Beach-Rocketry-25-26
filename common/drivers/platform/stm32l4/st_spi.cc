/**
 * @file st_spi.cc
 * @author Kent Hong
 * @brief SPI Driver class implementation
 * @date 2025-09-30
 *
 */

#include "st_spi.h"
#include "stm32l476xx.h"
namespace LBR
{
namespace Stml4
{

/**
 * @brief Sets any SPI register with the desired bits
 * 
 * @param reg 
 * @param enum_val 
 * @param bit_num 
 * @param bit_length 
 */
void HwSpi::SetReg(volatile uint32_t* reg, uint32_t enum_val, uint32_t bit_num,
                   uint32_t bit_length)
{
    uint32_t mask{(0x01 << bit_length) - 1U};
    *reg &= ~(mask << bit_num);
    *reg |= (mask & enum_val) << bit_num;
}

/**
 * @brief Checks the SPI object to see if it has valid control register settings
 *
 * @param spi The SPI object you want to check
 * @return true
 * @return false
 */
bool ValidateSpi(HwSpi& spi)
{
    // Check cr_settings
    if (static_cast<uint8_t>(spi.settings.baudrate) > 7)
    {
        return false;
    }
    if (static_cast<uint8_t>(spi.settings.busmode) > 3)
    {
        return false;
    }
    if (static_cast<uint8_t>(spi.settings.order) > 1)
    {
        return false;
    }
    if (static_cast<uint8_t>(spi.settings.threshold) > 1)
    {
        return false;
    }

    return true;
}

/**
 * @brief Construct a new HwSpi object
 *
 * @param instance_ The SPI peripheral being used
 * @param settings_ The SPI control register settings
 *
 */
HwSpi::HwSpi(SPI_TypeDef* instance_, StSpiSettings& settings_)
    : instance(instance_), settings(settings_)
{
}

/**
 * @brief Read data from a slave device.
 * 
 * @param rx_data 8 byte array to store read data.
 * @param buffer_len Size of array.
 * @return true 
 * @return false 
 */
bool HwSpi::Read(uint8_t* rx_data, size_t buffer_len)
{
    // Check if SPI is enabled
    if (!(instance->CR1 & SPI_CR1_SPE))
    {
        return false;
    }

    // Check if SPI is already in communication
    if (instance->SR & SPI_SR_BSY)
    {
        return false;
    }

    for (size_t i = 0; i < buffer_len; i++)
    {
        // Wait until TX Buffer is empty before sending dummy byte to generate clock pulse
        while (!(instance->SR & SPI_SR_TXE))
        {
        }

        // Send dummy byte to DR to generate clock pulse
        *(volatile uint8_t*)&instance->DR = 0x00;

        // Wait until RX buffer has enough data to be read
        while (!(instance->SR & SPI_SR_RXNE))
        {
        }

        // Read data from RX buffer
        rx_data[i] = *(volatile uint8_t*)&instance->DR;
    }

    // Wait until transmission is complete
    while (instance->SR & SPI_SR_BSY)
    {
    }

    return true;
}

/**
 * @brief Write data to a slave device.
 * 
 * @param tx_data 8 byte array of the data to be sent.
 * @param buffer_len Size of array
 * @return true 
 * @return false 
 */
bool HwSpi::Write(uint8_t* tx_data, size_t buffer_len)
{
    // Check if SPI is enabled
    if (!(instance->CR1 & SPI_CR1_SPE))
    {
        return false;
    }

    // Check if SPI is already in communication
    if (instance->SR & SPI_SR_BSY)
    {
        return false;
    }

    for (size_t i = 0; i < buffer_len; i++)
    {
        // Wait until TX Buffer is empty
        while (!(instance->SR & SPI_SR_TXE))
        {
        }

        /*
         * Write to SPI Data Register (DR).
         * Automatically starts clock once data is written to the DR.
         */
        *(volatile uint8_t*)&instance->DR = tx_data[i];

        /*
         * Wait for RX buffer to be filled.
         * Even if only writing, RXNE flag has to be cleared before proceeding to the next byte write.
         */
        while (!(instance->SR & SPI_SR_RXNE))
        {
        }

        /*
         * Clear RXNE flag by performing a dummy read in the RX Buffer.
         * Dereferencing the data reg performs a read. Type casting to void explicitly discards the unused value.
         */
        (void)(*(volatile uint8_t*)&instance->DR);
    }

    // Wait until transmission is complete
    while (instance->SR & SPI_SR_BSY)
    {
    }

    return true;
}

/**
 * @brief Read and Write data to a slave device.
 * 
 * @param tx_data 8 byte array of the data to be sent.
 * @param rx_data 8 byte array to store read data.
 * @param buffer_len Size of arrays
 * @return true 
 * @return false 
 */
bool HwSpi::Transfer(uint8_t* tx_data, uint8_t* rx_data, size_t buffer_len)
{
    // Check if SPI is enabled
    if (!(instance->CR1 & SPI_CR1_SPE))
    {
        return false;
    }

    // Check if SPI is already in communication
    if (instance->SR & SPI_SR_BSY)
    {
        return false;
    }

    for (size_t i = 0; i < buffer_len; i++)
    {
        // Wait until TX Buffer is empty
        while (!(instance->SR & SPI_SR_TXE))
        {
        }

        /*
         * Write to SPI Data Register (DR).
         * Automatically starts clock once data is written to the DR.
         */
        *(volatile uint8_t*)&instance->DR = tx_data[i];

        /*
         * Wait for RX buffer to be filled.
         * Even if only writing, RXNE flag has to be cleared before proceeding to the next byte write.
         */
        while (!(instance->SR & SPI_SR_RXNE))
        {
        }

        // Read data from RX buffer
        rx_data[i] = *(volatile uint8_t*)&instance->DR;
    }

    // Wait until transmission is complete
    while (instance->SR & SPI_SR_BSY)
    {
    }

    return true;
}

/**
 * @brief Initializes SPI peripheral and its sck, mosi, miso, and nss pins
 *
 * @return SpiStatus::Ok initialization success
 * @return SpiStatus::InitErr initialization failed
 */
bool HwSpi::Init()
{

    // Set to Master Mode (Keep in master mode unless we want our STM32l4xx to be
    // used as a slave device). Master Mode sets our STM32l4xx to act as the
    // master device to initiate/end SPI communication and drive the clock signal.
    instance->CR1 |= SPI_CR1_MSTR;

    // Enable Full-Duplex Mode (Can send and receive data simultaneously through
    // MOSI and MISO)
    instance->CR1 &= ~(SPI_CR1_RXONLY);

    // Configure SPI sck Baudrate
    SetReg(&instance->CR1, uint32_t(settings.baudrate), 3, 3);

    // Configure the SPI Bus Mode
    SetReg(&instance->CR1, uint32_t(settings.busmode), 0, 2);

    // Configure Bit order
    SetReg(&instance->CR1, uint32_t(settings.order), 7, 1);

    // Determine FIFO reception threshold to see how many bits in RX Buffer
    // triggers an RXNE event
    SetReg(&instance->CR2, uint32_t(settings.threshold), 12, 1);

    // Configure the SPI data size to 8 bits
    instance->CR2 |= (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2);

    // Use software slave management and toggle SSI = 1 to pull NSS to high
    // Toggle SSI = 0 when we want to select the slave device for data transfer
    instance->CR1 |= SPI_CR1_SSM;
    instance->CR1 |= SPI_CR1_SSI;

    // Enable SPI peripheral
    instance->CR1 |= SPI_CR1_SPE;

    return true;
}

/**
 * @brief Virtual function override - Read with default parameters
 * @return true if successful, false otherwise
 */
bool HwSpi::Read()
{
    // Default implementation could read a single byte or return an error
    // Since we don't have default parameters, we'll return false for now
    return false;
}

/**
 * @brief Virtual function override - Write with default parameters
 * @return true if successful, false otherwise
 */
bool HwSpi::Write()
{
    // Default implementation could write a single byte or return an error
    // Since we don't have default parameters, we'll return false for now
    return false;
}

/**
 * @brief Virtual function override - Transfer with default parameters
 * @return true if successful, false otherwise
 */
bool HwSpi::Transfer()
{
    // Default implementation could transfer a single byte or return an error
    // Since we don't have default parameters, we'll return false for now
    return false;
}
}  // namespace Stml4
}  // namespace LBR
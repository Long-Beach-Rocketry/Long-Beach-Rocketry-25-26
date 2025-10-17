/**
 * @file st_spi.cc
 * @brief SPI Driver class implementation for STM32L4xx
 * @author Bex Saw
 * @date 2025-09-30
 */

#include "st_spi.h"
#include "stm32l476xx.h"

namespace LBR
{
namespace Stml4
{

/**
 * @brief Set bits in a SPI register
 * 
 * @param reg Target register
 * @param enum_val Enum value or bit pattern to set
 * @param bit_num Starting bit position
 * @param bit_length Number of bits to modify
 */
void HwSpi::SetReg(volatile uint32_t* reg, uint32_t enum_val, uint32_t bit_num,
                   uint32_t bit_length)
{
    uint32_t mask = ((1U << bit_length) - 1U);
    *reg = (*reg & ~(mask << bit_num)) | ((enum_val & mask) << bit_num);
}

/**
 * @brief Validate SPI configuration
 * 
 * @param spi SPI object to validate
 * @return true if configuration is valid
 * @return false otherwise
 */
bool ValidateSpi(HwSpi& spi)
{
    if (static_cast<uint8_t>(spi.settings.baudrate) > 7)
        return false;
    if (static_cast<uint8_t>(spi.settings.busmode) > 3)
        return false;
    if (static_cast<uint8_t>(spi.settings.order) > 1)
        return false;
    if (static_cast<uint8_t>(spi.settings.threshold) > 1)
        return false;
    return true;
}

/**
 * @brief Constructor for HwSpi object
 * 
 * @param instance_ SPI peripheral base address
 * @param settings_ SPI configuration settings
 */
HwSpi::HwSpi(SPI_TypeDef* instance_, const StSpiSettings& settings_)
    : instance(instance_), settings(settings_)
{
}

/**
 * @brief Initialize the SPI peripheral
 * 
 * @return true if initialized successfully
 * @return false otherwise
 */
bool HwSpi::Init()
{
    instance->CR1 |= SPI_CR1_MSTR;     // Master mode
    instance->CR1 &= ~SPI_CR1_RXONLY;  // Full-duplex

    SetReg(&instance->CR1, uint32_t(settings.baudrate), 3, 3);
    SetReg(&instance->CR1, uint32_t(settings.busmode), 0, 2);
    SetReg(&instance->CR1, uint32_t(settings.order), 7, 1);
    SetReg(&instance->CR2, uint32_t(settings.threshold), 12, 1);

    instance->CR2 |=
        (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2);  // 8-bit data
    instance->CR1 |= (SPI_CR1_SSM | SPI_CR1_SSI);      // Software NSS
    instance->CR1 |= SPI_CR1_SPE;                      // Enable SPI

    return true;
}

/**
 * @brief Read data from SPI slave
 * 
 * @param rx_data Pointer to receive buffer
 * @param buffer_len Number of bytes to read
 * @return true if successful
 * @return false otherwise
 */
bool HwSpi::Read(uint8_t* rx_data, size_t buffer_len)
{
    if (!(instance->CR1 & SPI_CR1_SPE) || (instance->SR & SPI_SR_BSY))
        return false;

    for (size_t i = 0; i < buffer_len; i++)
    {
        while (!(instance->SR & SPI_SR_TXE))
        {
        }
        *(volatile uint8_t*)&instance->DR = 0x00;  // Dummy write
        while (!(instance->SR & SPI_SR_RXNE))
        {
        }
        rx_data[i] = *(volatile uint8_t*)&instance->DR;
    }

    while (instance->SR & SPI_SR_BSY)
    {
    }
    return true;
}

/**
 * @brief Write data to SPI slave
 * 
 * @param tx_data Pointer to data buffer
 * @param buffer_len Number of bytes to send
 * @return true if successful
 * @return false otherwise
 */
bool HwSpi::Write(const uint8_t* tx_data, size_t buffer_len)
{
    if (!(instance->CR1 & SPI_CR1_SPE) || (instance->SR & SPI_SR_BSY))
        return false;

    for (size_t i = 0; i < buffer_len; i++)
    {
        while (!(instance->SR & SPI_SR_TXE))
        {
        }
        *(volatile uint8_t*)&instance->DR = tx_data[i];
        while (!(instance->SR & SPI_SR_RXNE))
        {
        }
        (void)(*(volatile uint8_t*)&instance->DR);  // Clear RXNE
    }

    while (instance->SR & SPI_SR_BSY)
    {
    }
    return true;
}

/**
 * @brief Perform full-duplex SPI transfer
 * 
 * @param tx_data Pointer to transmit buffer
 * @param rx_data Pointer to receive buffer
 * @param buffer_len Number of bytes to transfer
 * @return true if successful
 * @return false otherwise
 */
bool HwSpi::Transfer(const uint8_t* tx_data, uint8_t* rx_data,
                     size_t buffer_len)
{
    if (!(instance->CR1 & SPI_CR1_SPE) || (instance->SR & SPI_SR_BSY))
        return false;

    for (size_t i = 0; i < buffer_len; i++)
    {
        while (!(instance->SR & SPI_SR_TXE))
        {
        }
        *(volatile uint8_t*)&instance->DR = tx_data[i];
        while (!(instance->SR & SPI_SR_RXNE))
        {
        }
        rx_data[i] = *(volatile uint8_t*)&instance->DR;
    }

    while (instance->SR & SPI_SR_BSY)
    {
    }
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

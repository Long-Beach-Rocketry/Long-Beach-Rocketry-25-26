/**
 * @file st_spi.cc
 * @author Kent Hong
 * @brief SPI Driver class implementation
 * @version 0.1
 * @date 2025-09-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "st_spi.h"
namespace LBR {
    namespace Stml4 {
        /**
         * @brief Construct a new SPI::SPI object
         * 
         * @param instance The SPI peripheral being used
         * @param nss      The desired nss GPIO from the pin table in st_spi_pins.h
         * @param sck      The desired sck GPIO from the pin table in st_spi_pins.h
         * @param miso     The desired miso GPIO from the pin table in st_spi_pins.h
         * @param mosi     The desired mosi GPIO from the pin table in st_spi_pins.h
         * 
         */
        SPI::SPI(SPI_Typedef* instance, const SpiPinMap& sck, const SpiPinMap& miso, const SpiPinMap& mosi) : 
        instance_(instance), 
        sck_pin_(CreateSpiPinSettings(sck.af), sck.pin, sck.port), 
        miso_pin_(CreateSpiPinSettings(miso.af), miso.pin, miso.port), 
        mosi_pin_(CreateSpiPinSettings(mosi.af), mosi.pin, mosi.port) {}

        SPI::SPI(SPI_Typedef* instance, const SpiPinMap& nss, const SpiPinMap& sck, const SpiPinMap& miso, const SpiPinMap& mosi) : 
        instance_(instance),
        nss_pin(CreateSpiPinSettings(nss.af), nss.pin, nss.port), 
        sck_pin_(CreateSpiPinSettings(sck.af), sck.pin, sck.port), 
        miso_pin_(CreateSpiPinSettings(miso.af), miso.pin, miso.port), 
        mosi_pin_(CreateSpiPinSettings(mosi.af), mosi.pin, mosi.port) {}
        
        /**
         * @brief Sets the SPI serial clock baudrate for bits 5:3 in SPIx_CR1
         * 
         * @param baudrate 
         */
        bool SPI::SetSpiBaudRate(SpiBaudRate baudrate) {

            // Clear bits 5:3 to 0b000
            instance_->CR1 &= ~(SPI_CR1_BR);

            switch(baudrate) {
                case SpiBaudRate::FPCLK_2:
                    break;                                           // 0b000 << 3
                case SpiBaudRate::FPCLK_4:
                    instance_->CR1 |= SPI_CR1_BR_0;                  // 0b001 << 3
                    break;
                case SpiBaudRate::FPCLK_8:
                    instance_->CR1 |= SPI_CR1_BR_1;                  // 0b010 << 3
                    break;
                case SpiBaudRate::FPCLK_16:
                    instance_->CR1 |= (SPI_CR1_BR_1 | SPI_CR1_BR_0); // 0b011 << 3
                    break;
                case SpiBaudRate::FPCLK_32:
                    instance_->CR1 |= SPI_CR1_BR_2;                  // 0b100 << 3
                    break;
                case SpiBaudRate::FPCLK_64:
                    instance_->CR1 |= (SPI_CR1_BR_2 | SPI_CR1_BR_0); // 0b101 << 3
                    break;
                case SpiBaudRate::FPCLK_128:
                    instance_->CR1 |= (SPI_CR1_BR_2 | SPI_CR1_BR_1); // 0b110 << 3
                    break;
                case SpiBaudRate::FPCLK_256:
                    instance_->CR1 |= SPI_CR1_BR;                    // 0b111 << 3
                    break;
                default:
                    return false;
                
            }
            return true;
        }

        /**
         * @brief Sets the SPI Bus Mode for Bit 1 - CPOL and Bit 0 - CPHA
         * 
         * @param mode The bus mode from the SpiBusMode enum class
         * @return true Bus Mode Configuration success
         * @return false Bus Mode Configuration failed
         * 
         * @note The bus mode is dependent on the slave device you are interfacing with (in other words, check its datasheet)
         */
        bool SPI::SetSpiBusMode(SpiBusMode mode) {

            switch(mode) {
                case SpiBusMode::MODE1:
                    instance_->CR1 &= ~(SPI_CR1_CPHA);   // CPHA = 0
                    instance_->CR1 &= ~(SPI_CR1_CPOL);   // CPOL = 0
                    break;
                case SpiBusMode::MODE2:
                    instance_->CR1 |= SPI_CR1_CPHA;      // CPHA = 1
                    instance_->CR1 &= ~(SPI_CR1_CPOL);   // CPOL = 0
                    break;
                case SpiBusMode::MODE3:
                    instance_->CR1 &= ~(SPI_CR1_CPHA);   // CPHA = 0
                    instance_->CR1 |= SPI_CR1_CPOL;      // CPOL = 1
                    break;
                case SpiBusMode::MODE4:
                    instance_->CR1 |= SPI_CR1_CPHA;      // CPHA = 1
                    instance_->CR1 |= SPI_CR1_CPOL;      // CPOL = 1
                default:
                    return false;
            }
            return true;
        }

        /**
         * @brief Sets the SPI data size before reading, writing, or transferring
         * 
         * @param datasize The number of bits from the SpiDataSize enum class
         * @return true Data Size Config success
         * @return false Data Size Config failed
         */
        bool SPI::SetSpiDataSize(SpiDataSize datasize) {

            // Clear bits 11:8 to 0b0000
            instance_->CR2 &= ~(SPI_CR2_DS);

            switch(datasize) {
                case SpiDataSize::ONE_BIT:
                    instance_->CR2 |= (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2);
                    break;
                case SpiDataSize::TWO_BIT:
                    instance_->CR2 |= (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2);
                    break;
                case SpiDataSize::THREE_BIT:
                    instance_->CR2 |= (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2);
                    break;
                case SpiDataSize::FOUR_BIT:
                    instance_->CR2 |= (SPI_CR2_DS_0 | SPI_CR2_DS_1);
                    break;
                case SpiDataSize::FIVE_BIT:
                    instance_->CR2 |= SPI_CR2_DS_2;
                    break;
                case SpiDataSize::SIX_BIT:
                    instance_->CR2 |= (SPI_CR2_DS_2 | SPI_CR2_DS_0);
                    break;
                case SpiDataSize::SEVEN_BIT:
                    instance_->CR2 |= (SPI_CR2_DS_2 | SPI_CR2_DS_1);
                    break;
                case SpiDataSize::EIGHT_BIT:
                    instance_->CR2 |= (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2);
                    break;
                case SpiDataSize::NINE_BIT:
                    instance_->CR2 |= SPI_CR2_DS_3;
                    break;
                case SpiDataSize::TEN_BIT:
                    instance_->CR2 |= (SPI_CR2_DS_3 | SPI_CR2_DS_0);
                    break;
                case SpiDataSize::ELEVEN_BIT:
                    instance_->CR2 |= (SPI_CR2_DS_3 | SPI_CR2_DS_1);
                    break;
                case SpiDataSize::TWELVE_BIT:
                    instance_->CR2 |= (SPI_CR2_DS_3 | SPI_CR2_DS_1 | SPI_CR2_DS_0);
                    break;
                case SpiDataSize::THIRTEEN_BIT:
                    instance_->CR2 |= (SPI_CR2_DS_3 | SPI_CR2_DS_2);
                    break;
                case SpiDataSize::FOURTEEN_BIT:
                    instance_->CR2 |= (SPI_CR2_DS_3 | SPI_CR2_DS_2 | SPI_CR2_DS_0);
                    break;
                case SpiDataSize::FIFTEEN_BIT:
                    instance_->CR2 |= (SPI_CR2_DS_3 | SPI_CR2_DS_2 | SPI_CR2_DS_1);
                    break;
                case SpiDataSize::SIXTEEN_BIT:
                    instance_->CR2 |= SPI_CR2_DS;
                    break;
                default:
                    return false;
            }
            return true;
        }

        /**
         * @brief Configure SPI settings to implement into CR1 and CR2
         * 
         * @param cfg SpiCrSettings struct with desired config values for the registers in CR1 and CR2
         */
        void SPI::SpiConfigSettings(const SpiCrSettings& cfg) {
            settings_ = cfg;
        }

        /**
         * @brief Assign nss, sck, miso, and mosi member variables with the desired pinout you want from the pin tables in st_spi_pins.h
         * 
         * @param nss 
         * @param sck 
         * @param miso 
         * @param mosi 
         */
        void SPI::SetSpiPins(const SpiPinMap& nss, const SpiPinMap& sck, const SpiPinMap& miso, const SpiPinMap& mosi) {
            nss_pin_ = HwGpio{CreateSpiPinSettings(nss.af), nss.pin, nss.port};
            sck_pin_ = HwGpio{CreateSpiPinSettings(sck.af), sck.pin, sck.port};
            miso_pin_ = HwGpio{CreateSpiPinSettings(miso.af), miso.pin, miso.port};
            mosi_pin_ = HwGpio{CreateSpiPinSettings(mosi.af), mosi.pin, mosi.port};
        }

        /**
         * @brief Initializes SPI peripheral and its sck, mosi, miso, and nss pins
         * 
         * @return SpiStatus::Ok initialization success
         * @return SpiStatus::InitErr initialization failed
         */
        SpiStatus SPI::Init() {

            // TODO: Implement functionality to enable GPIO clocks based on what pins were set in SetSpiPins()
            if (instance_ == SPI1) {
                // Enable GPIO G clock (PA 4, 5, 6, 7) AF5
                RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
                // Enable SPI1 clock
                RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
            } else if (instance_ == SPI2) {
                // Enable GPIO D clock (PD 0, 1, 3, 4) AF5
                RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;
                // Enable SPI2 clock
                RCC->APB1ENR1 |= RCC_APB1ENR1_SPI2EN;
            } else if (instance_ == SPI3) {
                // Enable GPIO G clock (PG 9, 10, 11, 12) AF6
                RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN;
                // Enable SPI3 clock
                RCC->APB1ENR1 |= RCC_APB1ENR1_SPI3EN;
            }

            // Set to Master Mode (Keep in master mode unless we want our STM32l4xx to be used as a slave device)
            // Master Mode - Sets our STM32l4xx to act as the master device to initiate/end SPI communication and drive the clock signal
            instance_->CR1 |= SPI_CR1_MSTR;

            // Enable Full-Duplex Mode (Can send and receive data simultaneously through MOSI and MISO)
            instance_->CR1 &= ~(SPI_CR1_RXONLY);

            // Configure SPI Baudrate
            if (!SetSpiBaudRate(settings_.baudrate)) {
                return SpiStatus::INIT_ERR;
            }

            // Configure SPI Bus Mode
            if (!SetSpiBusMode(settings_.mode)) {
                return SpiStatus::INIT_ERR;
            }

            // Configure SPI data size
            if (!SetSpiDataSize(settings_.datasize)) {
                return SpiStatus::INIT_ERR;
            }

            // Use software slave management and toggle SSI = 1 to pull NSS to high
            // Toggle SSI = 0 when we want to select the slave device for data transfer
            instance_->CR1 |= SPI_CR1_SSM;
            instance_->CR1 |= SPI_CR1_SSI;
            

            // Configure Bit order
            if (settings_.order == SpiBitOrder::MSB) {
                instance_->CR1 &= ~(SPI_CR1_LSBFIRST);
            } else if (settings_.order == SpiBitOrder::LSB) {
                instance_->CR1 |= SPI_CR1_LSBFIRST;
            }

            // Determine FIFO reception threshold to see how many bits in RX Buffer triggers an RXNE event
            if (settings_.threshold == SpiRxThreshold::FIFO_16bit) {
                instance_->CR2 &= ~(SPI_CR2_FRXTH);
            } else if (settings_.threshold == SpiRxThreshold::FIFO_8bit) {
                instance_->CR2 |= SPI_CR2_FRXTH;
            }
            
            // Init GPIO Pins
            sck_pin_.init();
            mosi_pin_.init();
            miso_pin_.init();
            nss_pin_.init();

            // Enable SPI peripheral
            instance_->CR1 |= SPI_CR1_SPE;

            return SpiStatus::OK;
        }
    }
}
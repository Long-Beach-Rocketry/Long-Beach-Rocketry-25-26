#include 'i2c.h'
namespace LBR {
    namespace Stml4 {
        I2C::I2C(I2C_TypeDef* instance, uint32_t timing, HwGpio gpio1, HwGpio gpio2) { 
            _instance = instance;
            _timing = timing;
            _gpio1 = gpio1;
            _gpio2 = gpio2;
        }

        I2C::Init() {
            /**
             * @brief Initializes I2C peripheral and corresponding GPIO pins
             * 
             * @param None
             * @return None
             */

            // Enabling clocks and configuring pins
            if (_instance == I2C1) {
                RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
                RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;
            } else if (_instance == I2C2) {
                RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
                RCC->APB1ENR1 |= RCC_APB1ENR1_I2C2EN;
            } else if (_instance == I2C3) {
                RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
                RCC->APB1ENR1 |= RCC_APB1ENR1_I2C3EN;
            } 

            gpio1.init();
            gpio2.init()

            // Reset peripheral
            _instance->CR1 &= ~I2C_CR1_PE;

            // Configure timing
            _instance->TIMINGR |= _timing;

            // Enable peripheral
            _instance->CR1 |= I2C_CR1_PE;

        }

        I2C::SetTarget(uint8_t DevAddr) {
            /**
             * @brief Set address of external device to be communicated with
             * 
             * @param DevAddr Address of device
             * @return true if successful, false otherwise
             */

            // Check if communication in progress
            if (_instance->CR2 & I2C_CR2_START) {
                return false;
            }

            _instance->CR2 &= ~I2C_CR2_ADD10;          // 7-bit addressing mode
            _instance->CR2 &= ~I2C_CR2_SADD;           // Clearing address
            _instance->CR2 |= (DevAddr << 1);          // Setting address

            return true;
        }

        I2C::Read(uint8_t* data, uint8_t size) {
            /**
             * @brief Read data from external device
             * 
             * @param data memory location storing data to be read
             * @param size number of bytes to be read
             * @return true if successful, false otherwise
             */
            
            // Make sure I2C bus is idle
            if ((gpio1.read() == 0x0) || (gpio2.read() == 0x0)) {
                return false;
            }

            // Check if communication in progress
            if (_instance->CR2 & I2C_CR2_START) {
                return false;
            }

            // Configure for reading
            _instance->CR2 &= ~I2C_CR2_NBYTES;
            _instance->CR2 |= ((size << I2C_CR2_NBYTES_POS) | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND);

            // Initiate read
            _instance->CR2 |= I2C_CR2_START;

            // Read
            for (uint8_t i = 0; i < size; i++) {
                // Wait for transfer
                // TODO: prob a better way to do this
                while (!(_instance->ISR & I2C_ISR_RXNE)) {
                    // Timeout occurred, clear flag
                    // TODO: figure out timeout stuff
                    if (_instance->ISR & I2C_ISR_TIMEOUT) {
                        _instance->ICR &= ~I2C_ICR_TIMEOUTCF;
                        return false;
                    }
                }

                data[i] = _instance->RXDR;
            }

            return true;
        }

        I2C::Write(const uint8_t* data, uint8_t size) {
            /**
             * @brief Writes data to external device
             * 
             * @param data memory location storing data to be written
             * @param size number of bytes to be written
             * @return true if successful, false otherwise
             */

            // Check if communication is in progress
            if (_instance->CR2 & I2C_CR2_START) {
                return false;
            }

            // Configure for writing
            _instance->CR2 &= ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN);
            _instance->CR2 |= ((size << I2C_CR2_NBYTES_POS) | I2C_CR2_AUTOEND);

            // Initiate write
            _instance->CR2 |= I2C_CR2_START;

            // Write
            for (uint8_t i = 0; i < size; i++) {
                // Wait for transfer
                // TODO: prob a better way to do this
                while (!(_instance->ISR & I2C_ISR_TXIS)) {
                    // Timeout occurred, clear flag
                    // TODO: figure out timeout stuff
                    if (_instance->ISR & I2C_ISR_TIMEOUT) {
                        _instance->ICR &= ~I2C_ICR_TIMEOUTCF;
                        return false;
                    }
                }

                _instance->TXDR = data[i];
            }

            return true;
        }
    }
}
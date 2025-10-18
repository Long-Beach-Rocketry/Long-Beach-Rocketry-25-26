/**
 * @file st_spi.cc
 * @brief spi implementation for STM32
 * @author Phoebe Ho
 * @date 10/2/2025
 *
 */


//spi implementation
#include "st_spi.h"
#include "stm32l476xx.h"
#include "st_gpio.h"

using namespace LBR::STM32 //telling compiler to use << namespace by default
/*
NAMESPACE:
- groups related code (classes, functions,variables) avoiding conflicts
- different than parent classes: Organizing groups while parent classes are for data structure 
- doesn't affect code, just groups them to organize (name wrapper)

WRAPPERS:
- makes it easier to use, but doesn't change functionality 
*/
void HwSpi::spi_init(SpiBaudRate baud, SpiEnable enable, SpiPinMode mode, SpiBitOrder bit){ //from HwSpi
    //enable SPI slave before master sends the CLK
    spi_->CR1 &= ~(0x7<<3);
    switch(baud){
        case SpiBaudRate::Baud2: spi_->CR1 |= 0x00; break; //it's 000 // !: Use defines from stm32l476xx.h for readability
        case SpiBaudRate::Baud4: spi_->CR1 |= 1<<3; break; //001
        case SpiBaudRate::Baud8: spi_->CR1 |= 1<<4; break; //010
        case SpiBaudRate::Baud16: spi_->CR1 |= (0x2<<3); break; //011
        case SpiBaudRate::Baud32: spi_->CR1 |= 1<<5; break; //100
        case SpiBaudRate::Baud64: spi_->CR1 |= (0x5<<3); break; //101
        case SpiBaudRate::Baud128: spi_->CR1 |= (0x3<<4); break; //110
        case SpiBaudRate::Baud256: spi_->CR1 |= (0x7<<3); break; //111
    }
    //CLK stops after last data transaction

    spi_->CR1 &= ~(0x1<<6); //inverts all bits so the 1 becomes a zero and &s with the original, so the 1 passed will be a zero
    //clearing bits^
    switch(enable){
    case SpiEnable::Enable: spi_->CR1 |= 1<<6; break; //enabling control register 
    case SpiEnable::Disable: spi_->CR1 &= ~(1<<6); break;//similar to an if statement //no disable so switch    
    /*
    case : each case corresponds to a constant value and if switch expression is equal, then it jumps to that case
    exit: exits switch system 
    */
    }
    spi_->CR1 &= ~(0x3<<1);
    switch(mode){
        case SpiPinMode::Mode1: spi_->CR1 |= 0x00; break; //clock phase is 0, clock polarity is 0 
        case SpiPinMode::Mode2: spi_->CR1 |= 1<<1; break; //clock phase is 0, clock polarity is 1
        case SpiPinMode::Mode3: spi_->CR1 |= 1<<0; break; //clock phase is 1, clock polarity 0
        case SpiPinMode::Mode4: spi_->CR1 |= 1<<0 | 1<<1; break; //clock phase is 1, clock polarity is 0
    }
    spi_->CR1 &= ~(1<<7);
    switch(bit){
        case SpiBitOrder::MSB_First:spi_->CR1 |= 0x00; break;
        case SpiBitOrder::LSB_First:spi_->CR1 |= 1<<7; break;
    }
    

}
void HwSpi::spi_read(){
    //checking if there's still data to receive RXNE not empty
    //checking if there's still data to receive: FIFO is not empty
    //while((spi_->SR & (3<<9)) ||(spi_->SR & (1<<10))||(spi_->SR & (1<<9)) ){ //when the first in first out reception is greater than 0
    while(!(spi_->SR & (1<<0))){
    } //waiting for data to receive and be read

    
    
    volatile uint8_t read_data = spi_->DR; //uint8_t is the data type; volatile: value can always change, read from hardware register
    //& signifies a reference to variable (allows function to directly modify original variable in calling code)
    //send in data read to spi_->DR
    if(spi_->SR & (1<<6)){ //checking for overrun (when master or slave receives too much data and doesn't have space to store it)
         volatile uint8_t discard = spi_->DR; //clearing overrun flag //stores it in a temporary variable discard
         //documentation: clear by rading spi->DR register ; SPI peripheral clears oldest value in RXFIFO , and if not read, it will be lost
    }}/*
    READ:
    - returns oldest value in RXFIFO that has not been read yet
    - managed by RXNE event (triggered when data stored in RXFIFO and threshold is reached)
    - when RXNE is cleared, RXFIFO is considered empty  
    - Aligned with RXFIFO configured by FRXTH 
    - 1. 
    */
   


void HwSpi::spi_write(uint8_t data){ 
        while(!(spi_->SR & (1<<1))) {//if TXE is empty, continues in the loop and doesn't move on

        }
        spi_->DR = data; //writing to SPI DR (data register) placed into FIFO

        
            
        
    while((spi_->SR & 1<<7)){ //BSY, TXE, etc are read, so do not set them 
    }//waiting till the BSY is false to move on
    /*

        while()  /*

    WRITE:
    - stores written data in TXFIFO at end of a send queue
    - Managed by the TXE event (triggered when TXFIFO level is less than or equal to half of its capacity)
    - TXE cleared --> TXFIFO is considered full
    
    */
}







    


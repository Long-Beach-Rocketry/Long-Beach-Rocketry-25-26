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
#include "st_gpio.h" //take out some
namespace LBR
{
namespace STM32
{
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
void HwSpi::spi_read(uint8_t* buffer_read, size_t length){ //pointer to buffer_read so any data will be stored there, length represents # of bytes (allows for more bytes to be read at a time instead of one at a time)
    //dummy byte to return byte from slave 0xFF --> all bits are high and just wants to clock out data from slave
    //need dummy byte: to receive any data from slave to keep clk - provides clock pulses
    //pointers allow modification of original 
    size_t i; //initializing i to use in the for loop (size_t represents unsigned (0 to 255) size/counts/lengths of an object)
    spi_->CR1 |= (1<<6); //enable spi - do we need?

//BSY FLAG -- add, dereference for volatile read data (convert to 8 bit register and access that register using *) &--> the address 
    //checking if there's still data to receive RXNE not empty
    //checking if there's still data to receive: FIFO is not empty
    //while((spi_->SR & (3<<9)) ||(spi_->SR & (1<<10))||(spi_->SR & (1<<9)) ){ //when the first in first out reception is greater than 0
    

    
    for(i = 0; i < length; i++){
          while(spi_->SR & SPI_SR_BSY){ //while(spi_->SR & 1<<7) <-- original one

    } //wait until bsy flag is off 
           while(!(spi_->SR & (1<<1))){ // waiting for TXE = making sure it's empty so it can read
    }
        //setting up dummy variable

    uint8_t dummy = 0xFF; //dummy variable to have clock pulses
    spi_->DR = dummy;
    while(!(spi_->SR & (1<<0))){
    } //waiting for data to receive and be read

    buffer_read[i] = spi_->DR; //uint8_t is the data type; volatile: value can always change, read from hardware register
    //& signifies a reference to variable (allows function to directly modify original variable in calling code)
    //send in data read to spi_->DR
    //spi_->DR should be volatile in hal
    if(spi_->SR & (1<<6)){ //checking for overrun (when master or slave receives too much data and doesn't have space to store it)
        uint8_t discard = spi_->DR; //clearing overrun flag //stores it in a temporary variable discard
         //documentation: clear by reading spi->DR register ; SPI peripheral clears oldest value in RXFIFO , and if not read, it will be lost
    }
    }
    /*
    READ:
    - returns oldest value in RXFIFO that has not been read yet
    - managed by RXNE event (triggered when data stored in RXFIFO and threshold is reached)
    - when RXNE is cleared, RXFIFO is considered empty  
    - Aligned with RXFIFO configured by FRXTH 
    - 1. 
    */
        while(spi_->SR & SPI_SR_BSY){ //while(spi_->SR & 1<<7) <-- original one

    } //wait until bsy flag is off 

}

void HwSpi::spi_write(uint8_t* data, size_t len){ 
    spi_->CR1 |= (1<<6); //enable spi
    size_t j;
      
    
  //ADD CHECK FOR BSY, implement for loop to keep feeding bytes you want to send from an array
    //every time it writes it needs to read (make sure things show up in read) -- checking if things can be written when 

        for(j = 0; j<len;j++){
              while(spi_->SR & SPI_SR_BSY){ //while(spi_->SR & 1<<7) <-- original one

    } //wait until bsy flag is off  //BSY, TXE, etc are read, so do not set them 
   
    
            while(!(spi_->SR & (1<<1))){ //waiting for TXE to be empty
            }
            spi_->DR = data[j]; //writing to SPI DR (data register) placed into FIFO
            }

        
            
        
    while(spi_->SR & SPI_SR_BSY){ //while(spi_->SR & 1<<7) <-- original one

    } //wait until bsy flag is off 
    /*

        while()  

    WRITE:
    - stores written data in TXFIFO at end of a send queue
    - Managed by the TXE event (triggered when TXFIFO level is less than or equal to half of its capacity)
    - TXE cleared --> TXFIFO is considered full
    
    */
}


void Hw::spi_transfer(){

}

}


//clangd --> to settings.json


}   
    


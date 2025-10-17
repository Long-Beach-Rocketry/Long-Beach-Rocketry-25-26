/**
 * @file st_spi.h
 * @brief spi header file for STM32
 * @author Phoebe Ho
 * @date 10/2/2025
 *
 */

//spi interface
//INIT Function
/*
initializes SPI Driver (internal data structures or pointers and specific driver implementations)
pin configurations: MOSI, MISO, SCK, CS/SS
clock configurations: enable peripheral clocks for SPI module and GPIO Ports (double check GPIO code)
Mode settings: baud rate, data order (MSB AND LSB), bits per transfer, and SPI mode 
spi peripheral : SPI1, SPI2, etc.
*/
#pragma once

#include "stm32l476xx.h"
constexpr int ST_GPIO_MAX_PINS = 16; //defining amt of pins for stm32
//using constexpr so the compiler can check (evaluated at compiler check)
//need to define function type when using constexpr

namespace LBR{
namespace STM32{
    
enum class SpiPinMode : std::uint8_t{ //defining modes of the pin 
    Mode1 = 0,
    Mode2,
    Mode3,
    Mode4
        };
enum class SpiBitOrder : std::uint8_t{
    MSB_First=0,
    LSB_First
};
/*enum class SpiError: std::uint8_t{
    //need to add
};*/
enum class SpiBaudRate: std::uint8_t{ // std::uint8_t is to ensure it's an unassigned integer of 8 bits
    Baud2 =0,
    Baud4,
    Baud8,
    Baud16,
    Baud32,
    Baud64,
    Baud128,
    Baud256
};


class HwSpi
{ //pins should already be configured
public: 
    // !: We dont need to set up the pins in the SPI class
    explicit HwSpi(SPI_TypeDef* spi_, uint16_t mosi, uint16_t miso, uint16_t clk); //passing SPI1, SPI2, etc
    //constructor declaration for a class: HwSpi ^ 
   //init function takes in no arguments --telling compiler it's meant to override a virtual function from a base 
    //class and if it's not found, it will generate an error (virtual so it can be overriden by derived classes)
    /*
    derived class: the new class created to inherit the parent class 
    ex: class dog : public Animal (dog is a derived class, so if you call Animal* a = new dog(), you
    actually call the functions of Animal instead, but it can override virtual methods if they're the same name) 
    */
   
     
    

private: //only accessible to the member functions of HwSpi (spi's own methods) (if i want initClocks(), need to call spi_init()
    //defining everything in the constructor
    SPI_TypeDef* const spi; //spi_ underscore is naming convention for private members
    //^ private pointer to hardware Spi peripheral 
    /*SPI_TypeDef
    - C struct defined in stm32l476xx.h
    - so we can use SPI1->CR1 |= SPI_CR1_SPE instead of mapping each one individually
    */


    
    bool spi_init(SpiBaudRate baud, SpiBitOrder bit, SpiEnable enable, SpiPinMode mode); 
    //NOTE: place in public or private?
    bool spi_read(uint8_t* buffer_read, size_t length); //want to return byte 
    bool spi_write(uint8_t* data, size_t len);
    bool spi_transfer(); // add parameters
        
    };
}; //semicolon after classes
}

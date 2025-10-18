//transfer(); //reads and writes at the same time 
/** 
 * @file SPI header
 * @author Phoebe Ho
 */
#include "stm32l4xx_hal_def.h"
#pragma once
namespace LBR{
    class spi{
        //virtual bool init()=0; don't need?
        virtual bool read(); //virtual bool: can be overridden in derived classes (child class)
        virtual bool write();
        virtual bool transfer(); // !: Suggestion - use Google C++ naming conventions
        ~spi() = default; //deconstructor; in case it goes out of bounds and set to default
    };
}
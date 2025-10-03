//transfer(); //reads and writes at the same time 
/** 
 * @file SPI header
 * @author Phoebe Ho
 */
#pragma once
namespace LBR{
    class spi{
        virtual bool read()=0; //virtual bool: can be overridden in derived classes (child class)
        virtual bool write();
        virtual bool transfer();
        ~spi() = default;
    };
}
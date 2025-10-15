/**
    * @file spi.h
    * @author Bex Saw  
    * @brief SPI API interface
    * @version 0.1
    * @date 2024-10-05
    * 
    * @copyright Copyright (c) 2025
    * 
    */

/** 
 * @brief The interface for SPI communication
        Read() - slave to master
        Write() - master to slave
        Transfer() - do both simultaneously 
*/

#pragma once

namespace LBR
{
class Spi
{
public:
    virtual bool Read()
    {
        return false;
    }
    virtual bool Write()
    {
        return false;
    }
    virtual bool Transfer()
    {
        return false;
    }
    virtual ~Spi() = default;
};
}  // namespace LBR

/**
 * @file spi.h
 * @author Kent Hong
 * @brief SPI API
 * @version 0.1
 * @date 2025-09-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once

namespace LBR {
    class Spi {
        public:
            virtual bool Read();
            virtual bool Write();
            virtual bool Transfer();
            ~Spi() = default;
    };
}
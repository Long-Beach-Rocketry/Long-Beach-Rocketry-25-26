/**
 * @file gpio.h
 * @brief Brief description of the file.
 *
 * Detailed description of what this file provides.
 * Can span multiple lines if necessary.
 */

#pragma once
namespace LBR
{
    /**
     * @class Gpio
     * @brief Brief description of the class.
     *
     * Longer description of the class and its purpose.
     */
    class Gpio
    {
    public:
        /**
         * @brief Example method that does something.
         * @param value Input value description.
         * @return Description of the return value.
         *
         * Detailed explanation if necessary.
         */
        virtual bool init() = 0;

        /**
         * @brief Example method that does something.
         * @param value Input value description.
         * @return Description of the return value.
         *
         * Detailed explanation if necessary.
         */
        virtual bool toggle() = 0;

        /**
         * @brief Example method that does something.
         * @param value Input value description.
         * @return Description of the return value.
         *
         * Detailed explanation if necessary.
         */
        virtual bool set(const bool active) = 0;

        /**
         * @brief Example method that does something.
         * @param value Input value description.
         * @return Description of the return value.
         *
         * Detailed explanation if necessary.
         */
        virtual bool read() = 0;

        ~Gpio() = default; // default constructor
    };
}

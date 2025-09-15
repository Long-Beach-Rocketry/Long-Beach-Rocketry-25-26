/**
 * @file gpio.h
 * @brief Gpio driver interface.
 * @author TJ Malaska
 * @date 9/13/2025
 *
 * Detailed description of what this file provides.
 * Can span multiple lines if necessary.
 */

#pragma once
namespace LBR
{
/**
 * @class Gpio
 * @brief Gpio driver instance.
 *
 * Longer description of the class and its purpose.
 */
class Gpio
{
public:
    /**
     * @brief Initializes gpio settings
     * @return Returns true if success.
     *
     * Detailed explanation if necessary.
     */
    virtual bool init() = 0;

    /**
     * @brief
     * @return Returns true if success.
     *
     * Detailed explanation if necessary.
     */
    //virtual bool toggle() = 0;

    /**
     * @brief
     * @param active bool to set gpio with.
     * @return Returns true if success.
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

    ~Gpio() =
        default;  // default destructor the constructor will be in the hw definition
};
}  // namespace LBR

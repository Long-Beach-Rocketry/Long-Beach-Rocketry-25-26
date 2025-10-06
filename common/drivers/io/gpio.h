/**
 * @file gpio.h
 * @brief Gpio driver interface.
 * @author TJ Malaska
 * @date 9/13/2025
 *
 */

#pragma once
namespace LBR
{
/**
 * @class Gpio
 * @brief Gpio driver instance.
 *
 * General Gpio interface.
 */
class Gpio
{
public:
    /**
     * @brief toggles pin.
     * @return Returns true if success.
     */
    virtual bool toggle() = 0;

    /**
     * @brief sets gpio state.
     * @param active bool to set gpio with.
     * @return Returns true if success.
     */
    virtual bool set(const bool active) = 0;

    /**
     * @brief Reads input register.
     * @return Returns bool of the input register.
     */
    virtual bool read() = 0;
    /* 
     * Default destructor. The constructor will be in the hw definition.
     */
    ~Gpio() = default;
};
}  // namespace LBR

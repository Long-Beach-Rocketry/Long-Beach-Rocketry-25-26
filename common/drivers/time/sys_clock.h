/**
 * @file gpio.h
 * @brief System clock interface.
 * @author TJ Malaska
 * @date 1//5/2026
 *
 */

#pragma once
namespace LBR
{
//hard code the clock set up first to test
/**
 * @class Clock
 * @brief
 */
class Clock
{
public:
    // /**
    //  * @brief
    //  * @return Returns true if success.
    //  */
    // virtual bool configure() = 0;

    /**
     * @brief 
     * @return Returns true if success.
     */
    virtual bool init_clock() = 0;

    virtual bool get_hz() = 0;
};
}  // namespace LBR
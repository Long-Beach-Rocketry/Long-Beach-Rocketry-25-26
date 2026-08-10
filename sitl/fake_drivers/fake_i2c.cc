#include "fake_I2c.h"

namespace LBR
{

FakeI2c::FakeI2c(FakeI2cSensors sensors_) : sensors{sensors_}
{
}

bool FakeI2c::init()
{
    if (sensors.bno055)
    {
        register_device(sensors.bno055);
    }

    if (sensors.bno055)
    {
        register_device(sensors.bmp390);
    }
}

bool FakeI2c::mem_read(std::span<uint8_t> data, const uint8_t reg_addr,
                       uint8_t dev_addr)
{

    // Check if device exists, if not return false
    InvI2cSensor* device = find_device(dev_addr);
    if (!device)
    {
        return false;
    }

    /* 
        Process the read of the reg_addr into the buffer.
        Increment the buffer edit index and the reg_addr to simulate 
        i2c auto-increment.
    */
    for (int i = 0; i < data.size(); i++)
    {
        uint8_t result;
        bool status = device->process_read(reg_addr + i, result);

        // If successful, store the read, otherwise return false
        if (status)
        {
            data[i] = result;
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool FakeI2c::mem_read(std::span<uint8_t> data, const uint16_t reg_addr,
                       uint8_t dev_addr)
{
    // Ensure the device exists by searching for the device address
    InvI2cSensor* device = find_device(dev_addr);
    if (!device)
    {
        return false;
    }

    /*
        Process read on each index of the buffer corresponding to the 
        reg_addr + index. The index (offset) is added to the reg_addr
        to simulate the i2c auto-increment.
    */
    for (int i = 0; i < data.size(); i++)
    {
        uint8_t result;
        bool status = device->process_read(reg_addr + i, result);

        // If the read was successful, store it. Otherwise return false
        if (status)
        {
            data[i] = result;
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool FakeI2c::mem_write(std::span<const uint8_t> data, const uint8_t reg_addr,
                        uint8_t dev_addr)
{

    // Ensure the device exists by looking for the device address
    InvI2cSensor* device = find_device(dev_addr);
    if (!device)
    {
        return false;
    }

    /*
        For each buffer index, process the write based on the index
        offset + the register address.
    */
    for (int i = 0; i < data.size(); i++)
    {
        // Return false if the operation is not successful
        if (!device->process_write(reg_addr + i, data[i]))
        {
            return false;
        }
    }
    return true;
}

bool FakeI2c::mem_write(std::span<const uint8_t> data, const uint16_t reg_addr,
                        uint8_t dev_addr)
{
    // Ensure that the device exists by searching for the device address
    InvI2cSensor* device = find_device(dev_addr);
    if (!device)
    {
        return false;
    }

    /*
        For each buffer index, process the write based on the index
        offset + the register address.
    */
    for (int i = 0; i < data.size(); i++)
    {
        // Return false if the write was unsuccesful
        if (!device->process_write(reg_addr + i, data[i]))
        {
            return false;
        }
    }
    return true;
}

bool FakeI2c::read(std::span<uint8_t> data, uint8_t dev_addr)
{
    // Ensure the device exists. If it does not, return false
    InvI2cSensor* device = find_device(dev_addr);
    if (!device)
    {
        return false;
    }

    /*
        For each index in the data buffer, read the reg_addr + index offset
        into it.
    */
    for (int i = 0; i < data.size(); i++)
    {
        uint8_t result;
        bool status = device->process_read(result);

        if (status)
        {
            data[i] = result;
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool FakeI2c::write(std::span<const uint8_t> data, uint8_t dev_addr)
{
    InvI2cSensor* device = find_device(dev_addr);
    if (!device)
    {
        return false;
    }

    for (int i = 0; i < data.size(); i++)
    {
        if (!device->process_write(data[i]))
        {
            return false;
        }
    }
    return true;
}

/**
 * @note The following are SITL specific methods.
 */

bool FakeI2c::register_device(InvI2cSensor* device)
{
    // Ensure not nullptr
    if (!device)
    {
        return false;
    }

    // Get the device's address.
    uint8_t dev_addr;
    if (device->get_8bit_addr(dev_addr))
    {
        /*
            .insert() returns <iterator, bool> where
            iterator points to the element and
            bool indicates successful insertion.

            .insert() will not replace the value of the key
            if the key already exists. The bool will be false
        */
        return devices_8addr.insert({dev_addr, device}).second;
    }
    return false;
}

InvI2cSensor* FakeI2c::find_device(uint8_t dev_addr) const
{
    /* 
        Search for the device address. If return .end(), it does not exist.
        Return the pointer to the inverse driver if it does, 
        otherwise nullptr.
    */
    auto it = devices_8addr.find(dev_addr);
    if (it == devices_8addr.end())
    {
        return nullptr;
    }
    return it->second;
}

}  // namespace LBR
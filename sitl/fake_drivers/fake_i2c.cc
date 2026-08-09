#include "fake_I2c.h"

namespace LBR
{

bool FakeI2c::mem_read(std::span<uint8_t> data, const uint8_t reg_addr,
                       uint8_t dev_addr)
{
    InvI2cSensor* device = find_device(dev_addr);
    if (!device)
    {
        return false;
    }

    for (int i = 0; i < data.size(); i++)
    {
        uint8_t result;
        bool status = device->process_read(reg_addr + i, result);

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
    InvI2cSensor* device = find_device(dev_addr);
    if (!device)
    {
        return false;
    }

    for (int i = 0; i < data.size(); i++)
    {
        uint8_t result;
        bool status = device->process_read(reg_addr + i, result);

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
    InvI2cSensor* device = find_device(dev_addr);
    if (!device)
    {
        return false;
    }

    for (int i = 0; i < data.size(); i++)
    {
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
    InvI2cSensor* device = find_device(dev_addr);
    if (!device)
    {
        return false;
    }

    for (int i = 0; i < data.size(); i++)
    {
        if (!device->process_write(reg_addr + i, data[i]))
        {
            return false;
        }
    }
    return true;
}

bool FakeI2c::read(std::span<uint8_t> data, uint8_t dev_addr)
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

bool FakeI2c::register_device(uint8_t dev_addr, InvI2cSensor* device)
{
    if (!device)
    {
        return false;
    }

    InvI2cSensor* it = find_device(dev_addr);
    if (it)
    {
        return false;
    }

    devices_8addr[dev_addr] = device;

    return true;
}

bool FakeI2c::register_device(uint16_t dev_addr, InvI2cSensor* device)
{
    if (!device)
    {
        return false;
    }

    InvI2cSensor* it = find_device(dev_addr);
    if (it)
    {
        return false;
    }

    devices_16addr[dev_addr] = device;

    return true;
}

InvI2cSensor* FakeI2c::find_device(uint8_t dev_addr) const
{
    auto it = devices_8addr.find(dev_addr);
    if (it == devices_8addr.end())
    {
        return nullptr;
    }
    return it->second;
}

InvI2cSensor* FakeI2c::find_device(uint16_t dev_addr) const
{
    auto it = devices_16addr.find(dev_addr);
    if (it == devices_16addr.end())
    {
        return nullptr;
    }
    return it->second;
}

}  // namespace LBR
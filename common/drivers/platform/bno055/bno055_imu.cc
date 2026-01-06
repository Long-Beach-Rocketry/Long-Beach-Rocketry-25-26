#include "bno055_imu.h"
#include <array>

namespace LBR
{
using LBR::Utils::DelayMs;

Bno055::Bno055(LBR::I2c& i2c, uint8_t addr) : i2c_(i2c), address_(addr)
{
}

/**
 * @brief Set the IMU operating mode.
 * @param mode Mode enum
 * @return true if successful, false otherwise
 */
bool Bno055::set_mode(Mode mode)
{
    std::array<uint8_t, 1> buf{static_cast<uint8_t>(mode)};
    bool ok = i2c_.mem_write(buf, REG_OPR_MODE, address_);
    // Per BNO055 datasheet, a delay is required after changing OPR_MODE
    // to allow the sensor to switch modes and stabilize.
    DelayMs(30);
    return ok;
}

/**
 * @brief Initialize and configure the IMU
 * 
 */
void Bno055::init()
{
    // Per BNO055 datasheet, allow time for device startup after power-on reset.
    DelayMs(650);

    uint8_t id = 0;
    i2c_.mem_read(std::span<uint8_t>(&id, 1), (uint8_t)0x00,
                  address_);  // CHIP_ID = 0x00
    for (int i = 0; i < 5 && id != 0xA0; i++)
    {
        // Per BNO055 datasheet, allow time for device to respond after reset.
        DelayMs(10);
        i2c_.mem_read(std::span<uint8_t>(&id, 1), (uint8_t)0x00, address_);
    }
    if (id != 0xA0)
        return;

    set_mode(Bno055::CONFIG);
    // Per BNO055 datasheet, delay after switching to CONFIG mode.
    LBR::Utils::DelayMs(25);

    uint8_t pwr_mode = 0x00;
    i2c_.mem_write(std::span<const uint8_t>(&pwr_mode, 1), (uint8_t)0x3E,
                   address_);  // PWR_MODE = 0x3E
    uint8_t page_id = 0x00;
    i2c_.mem_write(std::span<const uint8_t>(&page_id, 1), (uint8_t)0x07,
                   address_);  // PAGE_ID = 0x07

    set_mode(Bno055::IMU);
    // Per BNO055 datasheet, delay after switching to IMU mode.
    LBR::Utils::DelayMs(20);
}

/**
 * @brief Deinitialize the IMU and put it in low-power mode
 * 
 */
void Bno055::deinit()
{
    // Put device into deep suspend to save power
    set_mode(Bno055::CONFIG);
    // Per BNO055 datasheet, delay after switching to CONFIG mode.
    LBR::Utils::DelayMs(10);
    static constexpr uint8_t PWR_MODE_SUSPEND = 0x02;
    static constexpr uint8_t PWR_MODE_REG = 0x3E;
    std::array<uint8_t, 1> pwr_mode{PWR_MODE_SUSPEND};
    i2c_.mem_write(pwr_mode, PWR_MODE_REG, address_);
    // Per BNO055 datasheet, delay after entering suspend mode.
    LBR::Utils::DelayMs(25);
}

static inline int16_t combine(uint8_t lsb, uint8_t msb)
{
    return (static_cast<int16_t>(msb) << 8) | lsb;
}
/**
 * @brief Read all sensor data from the IMU
 * @param[out] out Output struct for sensor data
 * @return true if successful, false otherwise
 *
 */
bool Bno055::read_all(Bno055Data& out)
{
    uint8_t buf[6 + 6 + 6 + 6 + 8];  // ACC + GYR + LIA + GRAV + QUAT
    size_t idx = 0;

    // Read everything starting from ACC register (0x08)
    i2c_.mem_read(std::span<uint8_t>(buf, sizeof(buf)), (uint8_t)0x08,
                  address_);

    // Parse accel
    constexpr float ACCEL_SCALE = 100.0f;
    out.accel.x = combine(buf[0], buf[1]) / ACCEL_SCALE;
    out.accel.y = combine(buf[2], buf[3]) / ACCEL_SCALE;
    out.accel.z = combine(buf[4], buf[5]) / ACCEL_SCALE;
    idx += 12;  // Skip accel (6) + mag (6)

    // Parse gyro
    constexpr float GYRO_SCALE = 16.0f;
    out.gyro.x = combine(buf[idx + 0], buf[idx + 1]) / GYRO_SCALE;
    out.gyro.y = combine(buf[idx + 2], buf[idx + 3]) / GYRO_SCALE;
    out.gyro.z = combine(buf[idx + 4], buf[idx + 5]) / GYRO_SCALE;
    idx += 6;

    // Parse linear accel
    out.linear_accel.x = combine(buf[idx + 0], buf[idx + 1]) / ACCEL_SCALE;
    out.linear_accel.y = combine(buf[idx + 2], buf[idx + 3]) / ACCEL_SCALE;
    out.linear_accel.z = combine(buf[idx + 4], buf[idx + 5]) / ACCEL_SCALE;
    idx += 6;

    // Parse gravity
    out.gravity.x = combine(buf[idx + 0], buf[idx + 1]) / ACCEL_SCALE;
    out.gravity.y = combine(buf[idx + 2], buf[idx + 3]) / ACCEL_SCALE;
    out.gravity.z = combine(buf[idx + 4], buf[idx + 5]) / ACCEL_SCALE;
    idx += 6;

    // Parse quaternion
    constexpr float QUAT_SCALE = 16384.0f;
    out.quat.w = combine(buf[idx + 0], buf[idx + 1]) / QUAT_SCALE;
    out.quat.x = combine(buf[idx + 2], buf[idx + 3]) / QUAT_SCALE;
    out.quat.y = combine(buf[idx + 4], buf[idx + 5]) / QUAT_SCALE;
    out.quat.z = combine(buf[idx + 6], buf[idx + 7]) / QUAT_SCALE;

    return true;
}

bool Bno055::calibrate(uint8_t& value)
{
    static constexpr uint8_t CALIB_STAT_REG = 0x35;  // CALIB_STAT register
    return i2c_.mem_read(std::span<uint8_t>(&value, 1), CALIB_STAT_REG,
                         address_);
}

bool Bno055::get_sys_status(uint8_t& value)
{
    static constexpr uint8_t SYS_STATUS_REG = 0x39;  // SYS_STATUS register
    return i2c_.mem_read(std::span<uint8_t>(&value, 1), SYS_STATUS_REG,
                         address_);
}

bool Bno055::get_sys_error(uint8_t& value)
{
    static constexpr uint8_t SYS_ERR_REG = 0x3A;  // SYS_ERR register
    return i2c_.mem_read(std::span<uint8_t>(&value, 1), SYS_ERR_REG, address_);
}

bool Bno055::run_post(uint8_t& status)
{
    set_mode(Bno055::CONFIG);
    // Per BNO055 datasheet, delay after switching to CONFIG mode.
    LBR::Utils::DelayMs(25);
    uint8_t sys_trigger = 0x80;
    static constexpr uint8_t SYS_TRIGGER_REG = 0x3F;  // SYS_TRIGGER register
    i2c_.mem_write(std::span<const uint8_t>(&sys_trigger, 1), SYS_TRIGGER_REG,
                   address_);
    // Per BNO055 datasheet, delay for self-test completion.
    LBR::Utils::DelayMs(650);
    static constexpr uint8_t ST_RESULT_REG = 0x36;  // ST_RESULT register
    bool ok =
        i2c_.mem_read(std::span<uint8_t>(&status, 1), ST_RESULT_REG, address_);
    set_mode(Bno055::IMU);
    // Per BNO055 datasheet, delay after switching to IMU mode.
    LBR::Utils::DelayMs(20);
    return ok;
}

bool Bno055::run_bist(uint8_t& status)
{
    set_mode(Bno055::CONFIG);
    // Per BNO055 datasheet, delay after switching to CONFIG mode.
    LBR::Utils::DelayMs(25);
    uint8_t sys_trigger = 0x01;
    static constexpr uint8_t SYS_TRIGGER_REG = 0x3F;  // SYS_TRIGGER register
    i2c_.mem_write(std::span<const uint8_t>(&sys_trigger, 1), SYS_TRIGGER_REG,
                   address_);
    // Per BNO055 datasheet, delay for BIST completion.
    LBR::Utils::DelayMs(650);
    static constexpr uint8_t ST_RESULT_REG = 0x36;  // ST_RESULT register
    bool ok =
        i2c_.mem_read(std::span<uint8_t>(&status, 1), ST_RESULT_REG, address_);
    set_mode(Bno055::IMU);
    // Per BNO055 datasheet, delay after switching to IMU mode.
    LBR::Utils::DelayMs(20);
    return ok;
}

bool Bno055::get_chip_id(uint8_t& id)
{
    static constexpr uint8_t CHIP_ID_REG = 0x00;  // CHIP_ID register
    return i2c_.mem_read(std::span<uint8_t>(&id, 1), CHIP_ID_REG, address_);
}

bool Bno055::get_opr_mode(Mode& mode)
{
    uint8_t value = 0;
    bool ok =
        i2c_.mem_read(std::span<uint8_t>(&value, 1), REG_OPR_MODE, address_);
    if (ok)
    {
        mode = static_cast<Mode>(value);
    }
    return ok;
}

}  // namespace LBR
/**
* @file imu_app_bsp.h
* @brief Board Support Package for IMU application.
* @author Bex Saw
* @date 2025-10-24
*/

#pragma once

#include "st_i2c.h"
#include "bno055_imu.h"
#include "stm32l4xx.h"


LBR::Bno055& BSP_Init_IMU(I2C_TypeDef* i2c_instance, GPIO_TypeDef* gpio_instance);


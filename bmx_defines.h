#ifndef BMX_STRUCTS_H
#define BMX_STRUCTS_H

#include "stdio.h"
#include "stdint.h"
#include "hardware/i2c.h"

#define BMX_DEFAULT_ADDR 0x68

#define ACC_RATE_25_32 1
#define ACC_RATE_25_16 2
#define ACC_RATE_25_8 3
#define ACC_RATE_25_4 4
#define ACC_RATE_25_2 5
#define ACC_RATE_25 6
#define ACC_RATE_50 7
#define ACC_RATE_100 8
#define ACC_RATE_200 9
#define ACC_RATE_400 10
#define ACC_RATE_800 11
#define ACC_RATE_1600 12

#define ACC_RANGE_2 3
#define ACC_RANGE_4 5
#define ACC_RANGE_8 8
#define ACC_RANGE_16 12

#define GYR_RATE_25 6
#define GYR_RATE_50 7
#define GYR_RATE_100 8
#define GYR_RATE_200 9
#define GYR_RATE_400 10
#define GYR_RATE_800 11
#define GYR_RATE_1600 12

#define GYR_RANGE_2000 0
#define GYR_RANGE_1000 1
#define GYR_RANGE_500 2
#define GYR_RANGE_250 3
#define GYR_RANGE_125 4

#define SOFTRESET 0xB6
#define START_FOC 0x30
#define SET_ACC_PMU 0x10
#define SET_GYRO_PMU 0x14
#define SET_MAG_PMU 0x18

#define ACCEL_POWER_SUSPEND 0
#define ACCEL_POWER_NOMAL 1
#define ACCEL_LOW_POWER 2

#define ACCEL_CONF 0x20
#define GYRO_CONF 0x20

#define GYRO_POWER_SUSPEND 0
#define GYRO_POWER_NORMAL 1
#define GYRO_POWER_FAST_START 3

#define MAG_POWER_SUSPEND 0
#define MAG_POWER_NORMAL 1
#define MAG_POWER_LOW_POWER 2

#define FOC_DISABLE 0
#define FOC_1G 1
#define FOC_MINUS_1G 2
#define FOC_0G 3
#define FOC_GYRO_ENABLE 1
#define FOC_GYRO_DISABLE 0

#define PMU_MASK 0b00111111

#define ACC_DATA_REG 0x12
#define GYR_DATA_REG 0x0C
#define COMMAND_REG 0x7E
#define FOC_CONF_REG 0x69
#define GYRO_CONF_REG 0x42
#define GYRO_RANGE_REG 0x43
#define ACC_CONF_REG 0x40
#define ACC_RANGE_REG 0x41
#define OFFSET_REG 0x71

#define CONFIG_DEFAULT 0x20
typedef struct
{
    uint8_t foc_acc_x;
    uint8_t foc_acc_y;
    uint8_t foc_acc_z;
    uint8_t foc_gyr_enable;
} foc_conf;

typedef struct
{
    uint8_t accel_power_mode;
    uint8_t gyro_power_mode;
    uint8_t mag_power_mode;
} pmu_config;

typedef struct 
{
    //Address of BMX
    uint8_t addr;

    //I2C instance to which the sensor is connecte 
    i2c_inst_t *i2c;

    //FOC config
    foc_conf foc_config;

    // Accelerometer config
    uint8_t accel_rate;
    uint8_t accel_range;

    //Gyroscope Config
    uint8_t gyro_range;
    uint8_t gyro_rate;

    pmu_config power_mode;

} bmx_config;



#endif
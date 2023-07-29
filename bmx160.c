#include "bmx160.h"
#include "bmx_defines.h"

static inline void i2c_write_to_memory(bmx_config *bmx, uint8_t memory_addr, uint8_t *data, uint8_t len)
{
    uint8_t addr = memory_addr;
    i2c_write_blocking(bmx->i2c, bmx->addr, &addr, 1, true);
    for(int i =0; i<len; i++){
        i2c_write_blocking(bmx->i2c, bmx->addr, data, len, false);
    }
}

static inline uint8_t i2c_read_from_memory(bmx_config *bmx, uint8_t memory_addr, uint8_t *buf, uint8_t len)
{
    uint8_t addr = memory_addr;
    i2c_write_blocking(bmx->i2c, bmx->addr, &addr, 1, true);
    for(int i =0; i<len; i++)
    {
        *(buf + i) = i2c_read_blocking(bmx->i2c, bmx->addr, buf, len, false);
    }
}

void read_accel_data_raw(bmx_config *bmx, uint16_t *buf){
    uint8_t temp[6];
    i2c_read_from_memory(bmx, ACC_DATA_REG, temp, 6);
    for(int i =0; i<3; i++){
        *(buf + i) = (temp[2*i] << 8) | temp[2*i + 1];
    }
}

void read_gyro_data_raw(bmx_config *bmx, uint16_t *buf){
    uint8_t temp[6];
    i2c_read_from_memory(bmx, GYR_DATA_REG, temp, 6);
    for(int i =0; i<3; i++){
        *(buf + i) = (temp[2*i] << 8) | temp[2*i + 1];
    }
}

void convert_accel_data(bmx_config *bmx, uint16_t *data, double *acc_data){
    double k;
    switch(bmx->accel_range){
        case ACC_RANGE_2:
            k = 0.000598755;
            break;
        case ACC_RANGE_4:
            k = 0.00119751;
            break;
        case ACC_RANGE_8:
            k = 0.00239502;
            break;
        case ACC_RANGE_16:
            k = 0.004790039;
            break;
        default:
            k = 0.000598755;
            break;
    }

    for(int i=0; i<3; i++){
        *(acc_data + i) = (double)(*(data + i)) * k;
    }
}

void convert_gyrp_data(bmx_config *bmx, uint16_t *data, double *gyro_data){
    double k;
    switch(bmx->gyro_range){
        case GYR_RANGE_2000:
            k = 0.0610351;
            break;
        case GYR_RANGE_1000:
            k = 0.0305175;
            break;
        case GYR_RANGE_500:
            k = 0.0152587;
            break;
        case GYR_RANGE_250:
            k = 0.0076293;
            break;
        case GYR_RANGE_125:
            k = 0.0038147;
            break;
        default:
            k = 0.0076293;
            break;
    }

    for(int i=0; i<3; i++){
        *(gyro_data + i) = (double)(*(data + i)) * k;
    }
}

void read_gyro_data(bmx_config *bmx, double *buf)
{
    uint16_t temp[3];
    read_gyro_data_raw(bmx, temp);
    convert_gyro_data(bmx, temp, buf);
}

void read_accel_data(bmx_config *bmx, double * buf){
    uint16_t temp[3];
    read_accel_data_raw(bmx, temp);
    convert_accel_data(bmx, temp, buf);
}

/*****************************************
 * returns a config of bmx160 with default
 * Settings
 * ACCEL Range = +-2g
 * GYRO Range = +-250 deg/s
 * All sensors power mode set to normal
 * Intialized on I2C0 of Pi Pico
*/
bmx_config get_default_config(){
    bmx_config ret;
    ret.accel_range = ACC_RANGE_2;
    ret.accel_rate = ACC_RATE_400;
    ret.addr = BMX_DEFAULT_ADDR;
    ret.foc_en = false;
    ret.gyro_range = GYR_RANGE_250;
    ret.gyro_rate = GYR_RATE_400;
    ret.i2c = i2c0;
    ret.power_mode.accel_power_mode = ACCEL_POWER_NOMAL;
    ret.power_mode.gyro_power_mode = GYRO_POWER_NORMAL;
    ret.power_mode.mag_power_mode = MAG_POWER_NORMAL;
}

bool set_pmu(bmx_config *bmx){
    uint8_t data[3];
    data[0] = SET_ACC_PMU | bmx->power_mode.accel_power_mode;
    data[1] = SET_GYRO_PMU | bmx->power_mode.gyro_power_mode;
    data[2] = SET_MAG_PMU | bmx->power_mode.mag_power_mode;
    for(int i=0; i<3; i++){
        i2c_write_to_memory(bmx, COMMAND_REG, (data + i), 1);
    }
    uint8_t test = get_pmu_status(bmx) & PMU_MASK;
    uint8_t temp = (data[0] << 4 | data[1] << 2 | data[2] ) & PMU_MASK;
    if(test == temp) return true;
    else return false;
}

uint8_t get_pmu_status(bmx_config *bmx){
    uint8_t ret;
    i2c_read_from_memory(bmx, 0x03, &ret, 1);
    return ret;
}

bool init_sensor(bmx_config *bmx, uint8_t sck_pin, uint8_t sda_pin){
    bool ret = true;
    i2c_init(bmx->i2c, 400000);
    gpio_set_function(sck_pin, GPIO_FUNC_I2C);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sck_pin);
    gpio_pull_up(sda_pin);
    ret = set_pmu(bmx);
    return ret;
}


#include "bmx160.h"
#include "bmx_defines.h"

static void i2c_write_to_memory(bmx_config *bmx, uint8_t memory_addr, uint8_t data)
{
    uint8_t buf[2] = {memory_addr, data};
    i2c_write_blocking(bmx->i2c, bmx->addr, buf, 2, false);
}

static void i2c_read_from_memory(bmx_config *bmx, uint8_t memory_addr, uint8_t *buf, uint8_t len)
{
    uint8_t addr = memory_addr;
    i2c_write_blocking(bmx->i2c, bmx->addr, &addr, 1, true);
    i2c_read_blocking(bmx->i2c, bmx->addr, buf, len, false);
}

void read_accel_data_raw(bmx_config *bmx, uint16_t *buf){
    uint8_t temp[6];
    i2c_read_from_memory(bmx, ACC_DATA_REG, temp, 6);
    for(int i =0; i<3; i++){
        *(buf + i) = (temp[2*i + 1] << 8) | temp[2*i];
    }
}

void read_gyro_data_raw(bmx_config *bmx, uint16_t *buf){
    uint8_t temp[6];
    i2c_read_from_memory(bmx, GYR_DATA_REG, temp, 6);
    for(int i =0; i<3; i++){
        *(buf + i) = (temp[2*i + 1] << 8) | temp[2*i];
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
        *(acc_data + i) = (double)(int16_t)(*(data + i)) * k;
    }
}

void convert_gyro_data(bmx_config *bmx, uint16_t *data, double *gyro_data){
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
        *(gyro_data + i) = (double)(int16_t)(*(data + i)) * k;
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
    ret.gyro_range = GYR_RANGE_250;
    ret.gyro_rate = GYR_RATE_400;
    ret.i2c = i2c0;
    ret.power_mode.accel_power_mode = ACCEL_POWER_NOMAL;
    ret.power_mode.gyro_power_mode = GYRO_POWER_NORMAL;
    ret.power_mode.mag_power_mode = MAG_POWER_NORMAL;
    return ret;
}

bool set_pmu(bmx_config *bmx){
    uint8_t data[3];
    data[0] = SET_ACC_PMU | bmx->power_mode.accel_power_mode;
    data[1] = SET_GYRO_PMU | bmx->power_mode.gyro_power_mode;
    data[2] = SET_MAG_PMU | bmx->power_mode.mag_power_mode;
    for(int i=0; i<3; i++){
        i2c_write_to_memory(bmx, COMMAND_REG, data[i]);
        sleep_ms(80);
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

void init_sensor(bmx_config *bmx, uint8_t sck_pin, uint8_t sda_pin){
    bool ret = true;
    gpio_init(sck_pin);
    gpio_init(sda_pin);
    gpio_set_function(sck_pin, GPIO_FUNC_I2C);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sck_pin);
    gpio_pull_up(sda_pin);
    i2c_init(bmx->i2c, 400000);
}

void bmx_write_to(bmx_config *bmx, uint8_t reg, uint8_t data){
    i2c_write_to_memory(bmx, reg, data);
}

void bmx_foc(bmx_config *bmx){
    foc_conf temp = bmx->foc_config;
    uint8_t foc_reg = temp.foc_gyr_enable << 6 | temp.foc_acc_x << 4 | temp.foc_acc_y << 2 | temp.foc_acc_z;
    i2c_write_to_memory(bmx, FOC_CONF_REG, foc_reg & 0x7F);
    i2c_write_to_memory(bmx, COMMAND_REG, 0x03);
    sleep_ms(1000);
    uint8_t t;
    i2c_read_from_memory(bmx, 0x77, &t, 1);
    uint8_t foc_enable = 0;
    if(temp.foc_gyr_enable){
        if(temp.foc_acc_x | temp.foc_acc_y | temp.foc_acc_z){
            foc_enable = 0xC0;
        }else{
            foc_enable = 0x80;
        }
    }else if(temp.foc_acc_x | temp.foc_acc_y | temp.foc_acc_z){
        foc_enable = 0x40;
    }
    i2c_write_to_memory(bmx, 0x77, t | 0xC0);
}

void bmx_set_config(bmx_config *bmx){
    i2c_write_to_memory(bmx, ACC_CONF_REG, CONFIG_DEFAULT | bmx->accel_rate);
    i2c_write_to_memory(bmx, ACC_RANGE_REG, bmx->accel_range);
    i2c_write_to_memory(bmx, GYRO_CONF_REG, bmx->gyro_rate);
    i2c_write_to_memory(bmx, GYRO_RANGE_REG, bmx->gyro_range);
}

uint8_t bmx_read_from(bmx_config *bmx, uint8_t reg){
    uint8_t temp;
    i2c_read_from_memory(bmx, reg, &temp, 1);
    return temp;
}



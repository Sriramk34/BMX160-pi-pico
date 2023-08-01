#ifndef BMX160_H
#define BMX160_H

#include "stdio.h"
#include "stdint.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "bmx_defines.h"


//Write to register on BMX
static void i2c_write_to_memory(bmx_config *bmx, uint8_t memory_addr, uint8_t data);


//Read from register on BMX
static void i2c_read_from_memory(bmx_config *bmx, uint8_t memory_addr, uint8_t *buf, uint8_t len);


/*********************************************************
 * Read raw data from the accelerometer and the gyroscope.
 * Allocate 3 uinsigned 16 bit buffers
 ********************************************************/
void read_accel_data_raw(bmx_config *bmx, uint16_t *buf);

void read_gyro_data_raw(bmx_config *bmx, uint16_t *buf);

/************************************************
Convert raw data to processed data in the corect format
The data on the gyroscope output is in degrees per second and the accelerometer is in m/s^2
data is the raw data
***************************************************/
void convert_gyro_data(bmx_config *bmx, uint16_t *data, double *gyro_data); 

void convert_accel_data(bmx_config *bmx, uint16_t *data, double *acc_data);


/**************************************************
 * Read converted data
 * Read raw data and convert it before saving to buffer
*****************************************************/

void read_gyro_data(bmx_config *bmx, double *buf);

void read_accel_data(bmx_config *bmx, double * buf);

/*****************************************************
 * Set the config of the PMU register
 * Run at start of code to enable sensor
 * Return true if successfully set the resgister
******************************************************/
bmx_config get_default_config();

bool set_pmu(bmx_config *bmx);

uint8_t get_pmu_status(bmx_config *bmx);

/******************************************************
 * Perform self test and return true if test is passed
*******************************************************/
bool self_test_accel(bmx_config *bmx);

bool self_test_gyro(bmx_config *bmx);


/****************************************************
 * Setup pins for I2C for the BMX sensor
 * Run set PMU after this to start the sensor
*****************************************************/
void init_sensor(bmx_config *bmx, uint8_t sck_pin, uint8_t sda_pin);


/******************************************************
 * Perform FOC and save the data
 * Use settings from the foc conf in bmx
 * Make sure foc_en is set to true to perform FOC
*/
void bmx_foc(bmx_config *bmx);

/*****************************************************
 * Fucntion to write value to registers manually
******************************************************/
void bmx_write_to(bmx_config *bmx, uint8_t reg, uint8_t data);

uint8_t bmx_read_from(bmx_config *bmx, uint8_t reg);

void bmx_set_config(bmx_config *bmx);

void bmx_get_offset(bmx_config *bmx, uint8_t *accel, uint16_t *gyro);
#endif
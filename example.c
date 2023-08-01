#include <stdio.h>
#include "stdint.h"
#include "pico/stdlib.h"
#include "bmx160.h"

void init_bmx(bmx_config *bmx);

int main(){
    stdio_init_all();
    bmx_config bmx = get_default_config();
    init_bmx(&bmx);
    bool initialized = false;
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    //Allocate space to store accel and gyro data
    double accel[3];
    double gyro[3];
    while(true){
        gpio_put(25, 1 - gpio_get(25));
        read_accel_data(&bmx, accel);
        read_gyro_data(&bmx, gyro);
        printf("Accel X:%f   Y:%f   Z:%f\n", accel[0], accel[1], accel[2]);
        printf("Gyro X:%f   Y:%f   Z:%f\n", gyro[0], gyro[1], gyro[2]);
        sleep_ms(1000);
    }
}

void init_bmx(bmx_config *bmx){
    init_sensor(bmx, 5,4);
    bmx_set_config(bmx);
    set_pmu(&bmx);
    sleep_ms(1);
    //Set FOC config
    bmx->foc_config.foc_acc_x = FOC_0G;
    bmx->foc_config.foc_acc_y = FOC_0G;
    bmx->foc_config.foc_acc_z = FOC_1G;
    bmx->foc_config.foc_gyr_enable = FOC_GYRO_ENABLE;
    bmx_foc(bmx);
}
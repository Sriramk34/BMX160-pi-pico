#include <stdio.h>
#include "stdint.h"
#include "pico/stdlib.h"
#include "bmx160.h"

int main(){
    stdio_init_all();
    bmx_config bmx = get_default_config();
    bool test = test = init_sensor(&bmx, 5, 4);;
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    while(~test){
        sleep_ms(500);
        test = set_pmu(&bmx);
        if(test){
            printf("Successfully Initialized");
        }else{
            printf("Retrying to initialize sensor");
        }
    }
    
    double accel[3];
    double gyro[3];
    while(true){
        gpio_put(25, 1 - gpio_get(25));
        read_accel_data(&bmx, accel);
        read_gyro_data(&bmx, gyro);
        printf("Accel X:%f   Y:%f   Z:%f", accel[0], accel[1], accel[2]);
        printf("Gyro X:%f   Y:%f   Z:%f", gyro[0], gyro[1], gyro[2]);
        sleep_ms(1000);
    }
}
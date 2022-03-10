/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/init.h"
#include "board.h"
#include <aos/errno.h>
#include <aos/kernel.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "ulog/ulog.h"
#include "led.h"
#include "drv_acc_gyro_inv_mpu6050.h"
#include "drv_acc_gyro_qst_qmi8610.h"

#define LED_R LED1_NUM
#define LED_G LED2_NUM
#define LED_B LED3_NUM

#define SAMPLES_COUNT 20

#ifdef EDU_TAG
#undef EDU_TAG
#endif
#define EDU_TAG "eduk1_demo"

int counter = 0;
float samples[SAMPLES_COUNT];
int led_id = LED_R;              // 声明LED 设备 

int g_haasboard_is_k1c = 0;

static short r_ax = 0, r_ay = 0, r_az = 0;

int gyroscope_init(void)
{
    if (g_haasboard_is_k1c) {
        FisImu_init();
        LOGI(EDU_TAG, "FisImu_init done\n");
    } else {
        MPU_Init();
        LOGI(EDU_TAG, "MPU_Init done\n");
    }
    return 0;
}

static void sensor_refresh_task() {
    static int thiefFlag = 0;
    float acc[3];
    short ax, ay;
    int i;

    if (g_haasboard_is_k1c) {
        FisImu_read_acc_xyz(acc);
        ax = (short)acc[0];
        ay = (short)acc[1];
        printf("acc[0] = %f, acc[1] = %f\r\n", acc[0], acc[1]);
        printf("ax = %d, ay = %d\r\n", ax, ay);
        samples[counter % SAMPLES_COUNT] = (float)ax;
    } else {
        MPU_Get_Accelerometer(&r_ax, &r_ay, &r_az);
        printf("r_ax = %d, r_ay = %d, r_az = %d\r\n", r_ax, r_ay, r_az);
        samples[counter % SAMPLES_COUNT] = (float)r_ax;
    }
    
    if (counter >= SAMPLES_COUNT) {
        printf("Check if thief comes...\n");
        float sum = 0, avg, var = 0, diff;
        for (i = 0; i < SAMPLES_COUNT; i++) {
            sum += samples[i];
        }
        avg = sum / SAMPLES_COUNT; //求出平均的加速度
        printf("avg acc = %f\n", avg);
        for (i = 0; i < SAMPLES_COUNT; i++){
            diff = samples[i] - avg;
            var += diff * diff; // 求的是方差
        }
        printf("var = %f\n", var);
        if (var > SAMPLES_COUNT / 2) { // 阈值用户可以自己设定
            led_switch(led_id, LED_ON);// 点亮LED灯提醒用户有小偷 
            if (thiefFlag == 0) {
                printf("Thief!\n");
                thiefFlag = 1;
            }
        } else {
            led_switch(led_id, LED_OFF);
            printf("No Thief!\n");
            thiefFlag = 0;
        }
        counter = 0;
    }
    counter++;
}

int application_start(int argc, char *argv[])
{
    g_haasboard_is_k1c = haasedu_is_k1c();
    aos_set_log_level(AOS_LL_INFO);
    gyroscope_init();
    while (1) {
        sensor_refresh_task();
        aos_msleep(50);
    }
    return 0;
}

/*
 * Core2 for AWS IoT Kit BSP v2.1.0
 * Copyright (C) 2026 Rashed Talukder.  All Rights Reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file core2foraws_motion.c
 * @brief Core2 for AWS IoT Kit motion sensor hardware driver APIs
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include "mpu6886.h"
#include "core2foraws_common.h"
#include "core2foraws_motion.h"

static const char *_TAG = "CORE2FORAWS_MOTION";
static bool _motion_initialized = false;

esp_err_t core2foraws_motion_init( void )
{
    ESP_LOGI( _TAG, "\tInitializing" );

    if ( _motion_initialized )
    {
        return ESP_OK;
    }

    esp_err_t err = mpu6886_init( COMMON_I2C_INTERNAL );
    if ( err != ESP_OK )
    {
        ESP_LOGE( _TAG, "\tMPU6886 init failed: 0x%x", err );
    }
    else
    {
        _motion_initialized = true;
    }
    return err;
}

esp_err_t core2foraws_motion_temperature_get( float *temperature )
{
    if( temperature == NULL ) return ESP_ERR_INVALID_ARG;
    esp_err_t err = mpu6886_temp_data_get( temperature );
    if ( err == ESP_OK )
    {
        ESP_LOGV( _TAG, "temperature=%.2f C", *temperature );
    }
    return err;
}

esp_err_t core2foraws_motion_accel_get( float *x, float *y, float *z )
{
    if( x == NULL || y == NULL || z == NULL ) return ESP_ERR_INVALID_ARG;
    esp_err_t err = mpu6886_accel_data_get( x, y, z );
    if ( err == ESP_OK )
    {
        ESP_LOGV( _TAG, "accel x=%.3f y=%.3f z=%.3f g", *x, *y, *z );
    }
    return err;
}

esp_err_t core2foraws_motion_gyro_get( float *roll, float *pitch, float *yaw )
{
    if( roll == NULL || pitch == NULL || yaw == NULL )
        return ESP_ERR_INVALID_ARG;
    esp_err_t err = mpu6886_gyro_data_get( roll, pitch, yaw );
    if ( err == ESP_OK )
    {
        ESP_LOGV( _TAG, "gyro roll=%.3f pitch=%.3f yaw=%.3f dps", *roll, *pitch, *yaw );
    }
    return err;
}

esp_err_t core2foraws_motion_accel_gyro_get( float *x, float *y, float *z,
                                             float *roll, float *pitch,
                                             float *yaw )
{
    if( x == NULL || y == NULL || z == NULL ||
        roll == NULL || pitch == NULL || yaw == NULL )
        return ESP_ERR_INVALID_ARG;
    return mpu6886_accel_gyro_data_get( x, y, z, roll, pitch, yaw );
}

esp_err_t core2foraws_motion_accel_range_set( motion_accel_range_t range )
{
    if ( range < MOTION_ACCEL_RANGE_2G || range > MOTION_ACCEL_RANGE_16G )
    {
        return ESP_ERR_INVALID_ARG;
    }

    return mpu6886_fsr_accel_set( ( acc_scale_t )range );
}

esp_err_t core2foraws_motion_gyro_range_set( motion_gyro_range_t range )
{
    if ( range < MOTION_GYRO_RANGE_250DPS || range > MOTION_GYRO_RANGE_2000DPS )
    {
        return ESP_ERR_INVALID_ARG;
    }

    return mpu6886_fsr_gyro_set( ( gyro_scale_t )range );
}
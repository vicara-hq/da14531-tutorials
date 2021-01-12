/**
 ****************************************************************************************
 *
 * @file user_empty_peripheral_template.c
 *
 * @brief Empty peripheral template project source code.
 *
 * Copyright (C) 2012-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
#include "rwip_config.h"             // SW configuration


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"
#include "user_empty_peripheral_template.h"
#include "gpio.h"
#include "i2c.h"
#include "lsm6ds3_reg.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/
extern stmdev_ctx_t dev_ctx;
uint8_t accel_data_buffer[12] = {0};

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    default_app_on_connection(connection_idx, param);
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
}

void imu_init(void)
{
    uint8_t raw_data_buffer[14] = {0};

    uint8_t I2C_Response = 0xFF;
    lsm6ds3_int1_route_t int_1_reg;
    uint8_t who_am_i = 0;

    while(who_am_i != LSM6DS3_ID)
    {
        lsm6ds3_device_id_get(&dev_ctx, &who_am_i);
    }
    /* Reset IMU */
    lsm6ds3_reset_set(&dev_ctx, PROPERTY_ENABLE);
    do
    {
        lsm6ds3_reset_get(&dev_ctx, &I2C_Response);
    }
    while (I2C_Response);

    /* Enable Block Data Update */
    lsm6ds3_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);

    /* Set full scale */
    lsm6ds3_xl_full_scale_set(&dev_ctx, LSM6DS3_2g);
    lsm6ds3_gy_full_scale_set(&dev_ctx, LSM6DS3_2000dps);

    /* Set Output Data Rate */
    lsm6ds3_xl_data_rate_set(&dev_ctx, LSM6DS3_XL_ODR_12Hz5);
    lsm6ds3_gy_data_rate_set(&dev_ctx, LSM6DS3_GY_ODR_12Hz5);

    lsm6ds3_xl_power_mode_set(&dev_ctx, LSM6DS3_XL_NORMAL);
    lsm6ds3_gy_power_mode_set(&dev_ctx, LSM6DS3_GY_NORMAL);

    /* Set Interrupt Pin Active High */
    lsm6ds3_pin_polarity_set(&dev_ctx, LSM6DS3_ACTIVE_LOW);

    lsm6ds3_int_notification_set(&dev_ctx, LSM6DS3_INT_LATCHED);

    /* Set Accelerometer Data-Ready Interrupt */
    lsm6ds3_pin_int1_route_get(&dev_ctx, &int_1_reg);
    int_1_reg.int1_drdy_xl = PROPERTY_ENABLE;
    lsm6ds3_pin_int1_route_set(&dev_ctx, &int_1_reg);

    /* Synchronise Interrupt Signal */
    lsm6ds3_xl_flag_data_ready_get(&dev_ctx, &I2C_Response);
    do
    {
        lsm6ds3_xl_g_raw_get(&dev_ctx, raw_data_buffer);
        lsm6ds3_xl_flag_data_ready_get(&dev_ctx, &I2C_Response);
    }
    while(I2C_Response);
}

void interrupt_handler()
{
    lsm6ds3_xl_g_raw_get(&dev_ctx, accel_data_buffer);
    NVIC_ClearPendingIRQ(GPIO0_IRQn);
}

void user_app_on_init(void)
{
    imu_init();
    GPIO_EnableIRQ(GPIO_PORT_0, GPIO_PIN_7, GPIO0_IRQn, true, false, 0);
    GPIO_RegisterCallback(GPIO0_IRQn, interrupt_handler);
    default_app_on_init();
}
 

/// @} APP

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

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/
uint8_t last_led_state = 0;

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    default_app_on_connection(connection_idx, param);
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
}

void i2c_write(uint8_t reg, uint8_t *bufp, uint16_t len)
{

    i2c_abort_t abort_code;
    i2c_master_transmit_buffer_sync(&reg, 1, &abort_code, I2C_F_NONE);
    i2c_master_transmit_buffer_sync(bufp, len, &abort_code, I2C_F_ADD_STOP);
}

void i2c_read(uint8_t reg, uint8_t *bufp, uint16_t len)
{
    i2c_abort_t abort_code;
    i2c_master_transmit_buffer_sync(&reg, 1, &abort_code, I2C_F_NONE);
    i2c_master_receive_buffer_sync(bufp, len,  &abort_code, I2C_F_ADD_STOP);
}

void read_who_am_i_reg()
{
    uint8_t reg_val = 0;
    while(reg_val != 0x69)
    {
        i2c_read(0x0F, &reg_val, 1);
    }
}

void imu_reset()
{
    uint8_t reg_val = 0;
    i2c_read(0x12, &reg_val, 1);
    reg_val |= 0x01;
    i2c_write(0x12, &reg_val, 1);
}

void user_app_on_init(void)
{
    read_who_am_i_reg();
    imu_reset();
    read_who_am_i_reg();
    default_app_on_init();
}
 

/// @} APP

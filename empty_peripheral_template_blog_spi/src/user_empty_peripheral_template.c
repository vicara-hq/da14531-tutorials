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
#include "spi.h"

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

void read_who_am_i_reg()
{
     uint8_t reg_val = 0;
     while(reg_val != 0x69)
     {
         uint8_t reg_addr = 0x80|0x0F;
         spi_cs_low();
         spi_send(&reg_addr, 1, SPI_OP_BLOCKING);
         spi_receive(&reg_val, 1, SPI_OP_BLOCKING);
	 spi_cs_high();
     }
}

void user_app_on_init(void)
{
    read_who_am_i_reg();
    default_app_on_init();
}
 

/// @} APP

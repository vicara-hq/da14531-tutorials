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

void button_callback()
{
    if(last_led_state == 0)
    {
        last_led_state = 1;
        GPIO_SetActive(GPIO_PORT_0, GPIO_PIN_9);
    }
    else
    {
        last_led_state = 0;
        GPIO_SetInactive(GPIO_PORT_0, GPIO_PIN_9);
    }
}

void user_app_on_init(void)
{
    GPIO_EnableIRQ(GPIO_PORT_0, GPIO_PIN_11, GPIO0_IRQn, true, true, 150);
    GPIO_RegisterCallback(GPIO0_IRQn, button_callback);
    default_app_on_init();
}
 

/// @} APP

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
#include "timer0_2.h"
#include "timer0.h"
#include "gpio.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/
void timer_init();

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    default_app_on_connection(connection_idx, param);
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
}
 
void user_app_on_init()
{
    timer_init();
    default_app_on_init();
}

static void timer_irq_handler()
{
    if (GPIO_GetPinStatus(GPIO_PORT_0, GPIO_PIN_9))
    {
        GPIO_SetInactive(GPIO_PORT_0, GPIO_PIN_9);
    }
    else
    {
        GPIO_SetActive(GPIO_PORT_0, GPIO_PIN_9);
    }
}

void timer_init()
{

    static tim0_2_clk_div_config_t clk_div_config =
    {
        .clk_div  = TIM0_2_CLK_DIV_8
    };
    timer0_stop();
    timer0_register_callback(timer_irq_handler);
    timer0_2_clk_enable();
    timer0_2_clk_div_set(&clk_div_config);
    timer0_init(TIM0_CLK_FAST, PWM_MODE_ONE, TIM0_CLK_DIV_BY_10);

    timer0_set_pwm_high_counter(0);
    timer0_set_pwm_low_counter(0);

    timer0_set_pwm_on_counter(20000);
    timer0_enable_irq();
    timer0_start();

}
/// @} APP

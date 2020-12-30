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

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/
uint16_t on_cycles = 0;
uint16_t off_cycles = 100;

void pwm_init();

static tim0_2_clk_div_config_t clk_div_config =
{
    .clk_div  = TIM0_2_CLK_DIV_8
};

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
    pwm_init();
    default_app_on_init();
}

void timer_update()
{
	on_cycles += 2;
	off_cycles -= 2;
	
	if(on_cycles == 102)
	{
		on_cycles = 0;
		off_cycles = 100;
	}
	timer0_set_pwm_high_counter(on_cycles);
	timer0_set_pwm_low_counter(off_cycles);
}

void pwm_init()
{
    timer0_stop();
    timer0_register_callback(NULL);
    timer0_2_clk_enable();
    timer0_2_clk_div_set(&clk_div_config);
	//Clock input frequency is 16 Mhz / (8 * 10) = 200 khz
    timer0_init(TIM0_CLK_FAST, PWM_MODE_ONE, TIM0_CLK_DIV_BY_10);

	//SWTIM interrupt is triggered every 20000 * (200 khz) = 100 ms
    timer0_set(20000, on_cycles, off_cycles);
	timer0_register_callback(timer_update);

    timer0_enable_irq();
    timer0_start();

}
/// @} APP

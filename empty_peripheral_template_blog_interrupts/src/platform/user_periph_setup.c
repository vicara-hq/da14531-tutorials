/**
 ****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief Peripherals setup and initialization.
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "user_periph_setup.h"
#include "datasheet.h"
#include "system_library.h"
#include "rwip_config.h"
#include "gpio.h"
#include "uart.h"
#include "syscntl.h"
#include "i2c.h"
#include "lsm6ds3_reg.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
 
stmdev_ctx_t dev_ctx;

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
/*
    i.e. to reserve P0_1 as Generic Purpose I/O:
    RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);
*/
    RESERVE_GPIO(SDA, GPIO_PORT_0, GPIO_PIN_11, PID_I2C_SDA);
    RESERVE_GPIO(SCL, GPIO_PORT_0, GPIO_PIN_8, PID_I2C_SCL);
	RESERVE_GPIO(SCL, GPIO_PORT_0, GPIO_PIN_7, PID_GPIO);
#if defined (CFG_PRINTF_UART2)
    RESERVE_GPIO(UART2_TX, UART2_TX_PORT, UART2_TX_PIN, PID_UART2_TX);
#endif
}

#endif

void set_pad_functions(void)
{
/*
    i.e. to set P0_1 as Generic purpose Output:
    GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false);
*/
    GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_11, INPUT, PID_I2C_SDA, true);
    GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_8, OUTPUT, PID_I2C_SCL, true);
	GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_7, INPUT_PULLDOWN, PID_GPIO, false);
#if defined (__DA14586__)
    // Disallow spontaneous DA14586 SPI Flash wake-up
    GPIO_ConfigurePin(GPIO_PORT_2, GPIO_PIN_3, OUTPUT, PID_GPIO, true);
#endif

#if defined (CFG_PRINTF_UART2)
    // Configure UART2 TX Pad
    GPIO_ConfigurePin(UART2_TX_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false);
#endif

}

#if defined (CFG_PRINTF_UART2)
// Configuration struct for UART2
static const uart_cfg_t uart_cfg = {
    .baud_rate = UART2_BAUDRATE,
    .data_bits = UART2_DATABITS,
    .parity = UART2_PARITY,
    .stop_bits = UART2_STOPBITS,
    .auto_flow_control = UART2_AFCE,
    .use_fifo = UART2_FIFO,
    .tx_fifo_tr_lvl = UART2_TX_FIFO_LEVEL,
    .rx_fifo_tr_lvl = UART2_RX_FIFO_LEVEL,
    .intr_priority = 2,
};
#endif

int32_t platform_i2c_write(void *handle, uint8_t reg, uint8_t *bufp,
                              uint16_t len)
{
	
	i2c_abort_t abort_code;
	i2c_master_transmit_buffer_sync(&reg, 1, &abort_code, I2C_F_NONE);
	i2c_master_transmit_buffer_sync(bufp, len, &abort_code, I2C_F_ADD_STOP);
	return 0;
}

int32_t platform_i2c_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len)
{
	i2c_abort_t abort_code;
	i2c_master_transmit_buffer_sync(&reg, 1, &abort_code, I2C_F_NONE);
	i2c_master_receive_buffer_sync(bufp, len,  &abort_code, I2C_F_ADD_STOP);

	return 0;
}

void periph_init(void)
{
#if defined (__DA14531__)
    // In Boost mode enable the DCDC converter to supply VBAT_HIGH for the used GPIOs
    syscntl_dcdc_turn_on_in_boost(SYSCNTL_DCDC_LEVEL_3V0);
#else
    // Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));
    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);
#endif

    static const i2c_cfg_t i2c_cfg = {
    .clock_cfg.ss_hcnt = I2C_SS_SCL_HCNT_REG_RESET,
    .clock_cfg.ss_lcnt = I2C_SS_SCL_LCNT_REG_RESET,
    .clock_cfg.fs_hcnt = I2C_FS_SCL_HCNT_REG_RESET,
    .clock_cfg.fs_lcnt = I2C_FS_SCL_LCNT_REG_RESET,
    .restart_en = I2C_RESTART_ENABLE,
    .speed = I2C_SPEED_FAST,
    .mode = I2C_MODE_MASTER,
    .addr_mode = I2C_ADDRESSING_7B,
    .address = 0x6A,
    .tx_fifo_level = 1,
    .rx_fifo_level = 1,
    };

    // ROM patch
    patch_func();

    // Initialize peripherals
#if defined (CFG_PRINTF_UART2)
    // Initialize UART2
    uart_initialize(UART2, &uart_cfg);
#endif

    i2c_init(&i2c_cfg);
	
	// LSM6DS3 driver init
	dev_ctx.write_reg = platform_i2c_write;
	dev_ctx.read_reg = platform_i2c_read;
	dev_ctx.handle = NULL;

    // Set pad functionality
    set_pad_functions();

    // Enable the pads
    GPIO_set_pad_latch_en(true);
}

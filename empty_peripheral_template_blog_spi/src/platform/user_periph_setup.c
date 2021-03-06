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
#include "spi.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
/*
    i.e. to reserve P0_1 as Generic Purpose I/O:
    RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);
*/
    RESERVE_GPIO(SDA, GPIO_PORT_0, GPIO_PIN_11, PID_SPI_DO);
    RESERVE_GPIO(SCL, GPIO_PORT_0, GPIO_PIN_6, PID_SPI_DI);
    RESERVE_GPIO(SCL, GPIO_PORT_0, GPIO_PIN_8, PID_SPI_CLK);
    RESERVE_GPIO(SCL, GPIO_PORT_0, GPIO_PIN_7, PID_SPI_EN);
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
    GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_11, OUTPUT, PID_SPI_DO, true);
    GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_6, INPUT, PID_SPI_DI, true);
    GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_8, OUTPUT, PID_SPI_CLK, true);
    GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_7, OUTPUT, PID_SPI_EN, true);
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

    // ROM patch
    patch_func();

    // Initialize peripherals
#if defined (CFG_PRINTF_UART2)
    // Initialize UART2
    uart_initialize(UART2, &uart_cfg);
#endif

    // Configuration struct for SPI
    static const spi_cfg_t spi_cfg = {
        .spi_ms = SPI_MS_MODE_MASTER,
        .spi_cp = SPI_CP_MODE_0,
        .spi_speed = SPI_SPEED_MODE_2MHz,
        .spi_wsz = SPI_MODE_8BIT,
        .spi_cs = SPI_CS_0,
        .cs_pad.port = GPIO_PORT_0,
        .cs_pad.pin = GPIO_PIN_7,
    #if defined (CFG_SPI_DMA_SUPPORT)
        .spi_dma_channel = SPI_DMA_CHANNEL_01,
        .spi_dma_priority = DMA_PRIO_0,
    #endif
    };

    spi_initialize(&spi_cfg);

    // Set pad functionality
    set_pad_functions();

    // Enable the pads
    GPIO_set_pad_latch_en(true);
}

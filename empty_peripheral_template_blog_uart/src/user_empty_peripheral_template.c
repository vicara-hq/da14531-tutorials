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
#include "uart.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/
#define RX_BUFFER_LENGTH 5
uint8_t uart_rx_buffer[RX_BUFFER_LENGTH] = {0};

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    default_app_on_connection(connection_idx, param);
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
}

static void uart_rx_handler(uint16_t length)
{
    char print_str[] = "\r\nChar received = " ;
    uart_send(UART2, print_str, strlen(print_str), UART_OP_BLOCKING);
    uart_send(UART2, uart_rx_buffer, RX_BUFFER_LENGTH, UART_OP_BLOCKING);
}
 
void user_app_on_init()
{
    char print_str[] = "Enter 5 characters: ";
    default_app_on_init();
    uart_send(UART2, print_str, strlen(print_str), UART_OP_BLOCKING);
    uart_register_rx_cb(UART2, uart_rx_handler);
    uart_receive(UART2, uart_rx_buffer, RX_BUFFER_LENGTH, UART_OP_INTR);
}
/// @} APP

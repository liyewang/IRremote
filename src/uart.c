#include <stdint.h>
#include <stdbool.h>
#include "uart.h"
#include "driver.h"
#include "r05d.h"

void uart_send_hex_string(const uint8_t data)
{
    uint8_t data_H = data >> 4;
    uint8_t data_L = data & 0xf;
    if (data_H > 9)
        uart_send_data('A' + data_H - 10);
    else
        uart_send_data('0' + data_H);
    if (data_L > 9)
        uart_send_data('A' + data_L - 10);
    else
        uart_send_data('0' + data_L);
}



void uart_task(void)
{
    static uint8_t buffer[BUFF_SZ] = {0};
    static uint8_t buffer_idx = 0;
    // uint8_t input;
    uartTxRxEnable();
    if (uart_recv_ready())
    {
        // uart_recv_data(&input);
        // switch (input)
        // {
        //     case '\0':
        //     case '\r':
        //     case '\n':
        // }
        uart_recv_data(buffer + buffer_idx);
        uart_send_data(buffer[buffer_idx++]);
        if (buffer_idx >= 6)
        {
            R05D_tx(buffer, buffer_idx);
            buffer_idx = 0;
        }
    }
}
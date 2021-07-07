#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "uart.h"
#include "r05d.h"
#include "main.h"
#include "driver.h"

//-----------------------------------------------

// typedef enum
// {
//     NEC,
//     MIDEA_R05D
// } protocol_t;

/* define variables */
volatile task_t task = TASK_NONE;

// uint8_t data[NUM_DATA];

static void uart(void)
{
    // EX1 = 0;        // Disable external interrupt 1
    // while (REN);
    // for (uint8_t i = 0; i < NUM_DATA; i++)
    //     uart_send_data(data[i]);
    // task = TASK_IR_TX;
    uart_task();
}

static void ir_tx(void)
{
    // EX1 = 0;        // Disable external interrupt 1
    // ES = 0;         // Disable UART interrupt
    // ET0 = 1;        // Enable timer0 interrupt
    // ET1 = 1;        // Enable timer1 interrupt
    // // R05D_tx(0xb2, 0x7b, 0xe0);
    // R05D_tx(data);
    // task = TASK_NONE;
    // ET0 = 0;        // Disable timer0 interrupt
    // ET1 = 0;        // Disable timer1 interrupt
}

static void ir_rx(void)
{
    // R05D_rx();
    // // timer1_count = ~(uint16_t)0;
    // // signal = IR_MARK;
    // // EX1 = 0;        // Disable external interrupt 1
    // // ET1 = 1;        // Enable timer1 interrupt
    // // ES = 1;         // Enable UART interrupt
    // // timer1 = TMR16_10US + TIMER1_OFFSET;
    // // Timer1Set(TMR16_10US + TIMER1_OFFSET + 30);
    // // while (timer1_count > (TMR_MAX16 - IR_RX_TIMEOUT))
    // // {
    // //     if (IR_RX != signal)
    // //     {
    // //         uart_send_data(~(uint8_t)(timer1_count >> 8));
    // //         uart_send_data(~(uint8_t)timer1_count);
    // //         signal = IR_RX;
    // //         timer1_count = ~(uint16_t)0;
    // //     }
    // // }
    // task = TASK_NONE;
    // // ET1 = 0;        // Disable timer1 interrupt
    // // ES = 0;         // Disable UART interrupt
}

/* main program */
void main(void)
{
    hw_init();
    uart_send_string("IRremote v0.1\r\n");
    // task loop
    while (1)
    {
        if (task == TASK_NONE)
        {
            EX1 = 1;        // Enable external interrupt 1
            ES = 1;         // Enable UART interrupt
            REN = 1;
            task = TASK_UART;
        }
        else if (task == TASK_UART)
            uart();
        // else if (task == TASK_IR_TX)
        //     ir_tx();
        // else if (task == TASK_IR_RX)
        //     ir_rx();
    }
}

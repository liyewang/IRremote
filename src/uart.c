#include <stdbool.h>
#include "main.h"
#include "uart.h"

extern task_t task;
static bool busy;
uint8_t data[NUM_DATA];
static uint8_t data_idx = 0;
// static bool data_full = false;

/* Serial port interrupt routine */
INTERRUPT(serial_isr, SI0_VECTOR)
{
    if (RI)
    {
        RI = 0;             //Clear receive interrupt flag
        data[data_idx++] = SBUF;          //Save show UART data
        task = TASK_UATR;
        if (data_idx >= NUM_DATA)
        {
            REN = 0;
            data_idx = 0;
        }
#if (PARITYBIT != NONE_PARITY)
        bit9 = RB8;         //P2.2 show parity bit
#endif
    }
    if (TI)
    {
        TI = 0;             //Clear transmit interrupt flag
        busy = 0;           //Clear transmit busy flag
    }
}

void uart_init(void)
{
#if (PARITYBIT == NONE_PARITY)
    SCON = 0x50;            //8-bit variable UART
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    SCON = 0xda;            //9-bit variable UART, parity bit initial to 1
#elif (PARITYBIT == SPACE_PARITY)
    SCON = 0xd2;            //9-bit variable UART, parity bit initial to 0
#endif
    PCON |= 0x80;
    AUXR |= 0x15;
    BRT = (uint8_t)(TMR_MAX8 - SYSCLK / 16 / BAUD);
    // TMOD = 0x20;            //Set Timer1 as 8-bit auto reload mode
    // TH1 = TL1 = (uint8_t)(TMR_MAX8 - (SYSCLK / 32 / BAUD)); //Set auto-reload vaule
    // TR1 = 1;                //Timer1 start run
    ES = 1;                 // Enable UART interrupt
    EA = 1;                 // Open master interrupt switch
}

void uart_send_data(const uint8_t dat)
{
    while (busy);           //Wait for the completion of the previous data is sent
#if (PARITYBIT != NONE_PARITY)
    ACC = dat;              //Calculate the even parity bit P (PSW.0)
    if (P)                  //Set the parity bit according to P
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;            //Set parity bit to 0
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;            //Set parity bit to 1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;            //Set parity bit to 1
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;            //Set parity bit to 0
#endif
    }
#endif
    busy = 1;
#if (PARITYBIT != NONE_PARITY)
    SBUF = ACC;             //Send data to UART buffer
#elif (PARITYBIT == NONE_PARITY)
    SBUF = dat;             //Send data to UART buffer
#endif

}

void uart_send_string(const char* const str)
{
    const char* s = str;
    while (*s)                  //Check the end of the string
        uart_send_data(*s++);   //Send current char and increment string ptr
}

// void uart_recv_data(void)
// {}

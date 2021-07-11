#include <stdint.h>
#include <stdbool.h>
#include <stc12.h>
#include "driver.h"

volatile uint16_t timer0;
volatile uint16_t timer1;
volatile uint16_t timer1_count;
uint16_t timer_cycle_on;
uint16_t timer_cycle_off;
uint16_t recv_timeout;
volatile ir_signal_t signal;
volatile static ir_signal_t signal_next;
volatile static bool signal_loaded;
volatile bool IE1_flag;
volatile static bool tx_busy = false;
volatile bool rx_ready = false;
volatile static uint8_t rx_byte = 0;
#if (PARITYBIT != NONE_PARITY)
volatile static bool bit9;
#endif

void hw_init(void)
{
    IR_TX = IR_TX_OFF;
    LED = LED_OFF;
    Timer0Init;
    Timer1Init;
    setIntExt0(false);
    setModeExt1(ExtMode_Fall);
    uart_init();
    setIntGlobal(true);
}

/* IR */
void IRTxEnable(const bool en)
{
    if (en)
    {
        // setIntExt0(false);
        setIntExt1(false);
        setIntUart(false);
        setUartRecv(false);
        // setIntGlobal(true);
    }
    setIntTimer0(en);
    setIntTimer1(en);
}

void IRRxEnable(const bool en)
{
    if (en)
    {
        // setIntExt0(false);
        setUartRecv(false);
        setIntTimer0(false);
        // setIntGlobal(true);
    }
    setIntExt1(en);
    setIntUart(en);
    setIntTimer1(en);
}

/* Timer 0 interrupt routine */
INTERRUPT(timer0_isr, TF0_VECTOR)
{
    if (signal == IR_MARK)
    {
        // 582 clocks for 38kHz
        Timer0Set(timer0);
        // 38kHz 1/3 duty cycle pulse
        IR_TX = !IR_TX;
        LED = !LED;
        // prepare timer0 for the next INT
        if (IR_TX == IR_TX_ON)
        {
            timer0 = timer_cycle_off + TIMER0_OFFSET;
        }
        else
        {
            timer0 = timer_cycle_on + TIMER0_OFFSET;
        }
    }
    else
    {
        IR_TX = IR_TX_OFF;
        LED = LED_OFF;
    }
}

/* Timer 1 interrupt routine */
INTERRUPT(timer1_isr, TF1_VECTOR)
{
    Timer1Set(timer1);
    if (timer1_count)
    {
        timer1_count--;
    }
    else
    {
        signal = signal_next;
        if (signal == IR_MARK)
        {
            timer0 = timer_cycle_on;
        }
        else
        {
            TR0 = 0;
        }
        timer0_isr();
        signal_loaded = false;
    }
}

/* External interrupt 1 routine */
INTERRUPT(extern1_isr, IE1_VECTOR)
{
    Timer1Set(TMR16_1MS + TIMER1_OFFSET + TIMER1_OFFSET);
    timer1 = TMR16_1MS + TIMER1_OFFSET;
    timer1_count = (uint16_t)~0;
    IE1_flag = true;
}

void setTimerMS(const bool en)
{
    if (en)
    {
        timer1 = TMR16_1MS + TIMER1_OFFSET;
        timer1_count = (uint16_t)~0;
        setIntTimer1(true);
        // setIntGlobal(true);
        Timer1Set(TMR16_1MS + TIMER1_OFFSET);
    }
    else
    {
        setIntTimer1(false);
        timer1_count = 0;
    }
}

#pragma save
#pragma nogcse
void send_IR_signal(ir_signal_t sig_next, uint16_t sig_dur_rem, uint8_t sig_dur_mul)
{
    signal_next = sig_next;
    timer1 = sig_dur_rem + TIMER1_OFFSET;
    signal_loaded = true;
    while (signal_loaded);
    timer1_count = sig_dur_mul;
    timer1 = TIMER1_OFFSET;
    while (timer1_count);
}
#pragma restore

/* recv_IR_signal must be call just after extern1_isr has been called */
void recv_IR_signal(uint16_t* const mark_dur_us, uint16_t* const space_dur_us)
{
    uint16_t us;
    uint16_t ms;
    *mark_dur_us = 0;
    *space_dur_us = 0;
    while (timer1_count > (TMR_MAX16 - recv_timeout))
    {
        if (IR_RX == IR_RX_SPACE)
        {
            us = Timer1Get;
            ms = ~timer1_count * 1024;
            us = ((uint16_t)(us - TMR16_1MS) / (uint16_t)(SYSCLK / DIV_1US));
            if (*mark_dur_us == 0)
            {
                *mark_dur_us = ms + us;
            }
            *space_dur_us = ms + us - *mark_dur_us;
        }
        if (IE1_flag)
        {
            return;
        }
    }
    *space_dur_us = (uint16_t)~0;
}

// void pulseGen(void)
// {
//     // disable other interrupt
//     while (task == TASK_IR_TX)
//     {
//         if (signal == IR_MARK)
//         {
//             __asm
// __loop_inf:
//             // On state cycle
//             IR_TX = 1;      // 4 clocks
//             // 194 clocks delay for 1/3 duty cycle
//             mov	r7,#0x17    // 2 clocks
// __loop_on:
//             mov	a,r7        // 1 clock
//             dec	a           // 2 clocks
//             mov	r7,a        // 2 clocks
//             jnz	__loop_on   // 3 clocks
//             nop             // 1 clock
//             nop             // 1 clock
//             nop             // 1 clock
//             nop             // 1 clock
//             // Off state cycle
//             IR_TX = 0;      // 4 clocks
//             // 388 clocks delay for 2/3 duty cycle
//             mov	r7,#0x2f    // 2 clocks
// __loop_off:
//             mov	a,r7        // 1 clock
//             dec	a           // 2 clocks
//             mov	r7,a        // 2 clocks
//             jnz	__loop_off  // 3 clocks
//             nop             // 1 clock
//             nop             // 1 clock
//             nop             // 1 clock
//             sjmp __loop_inf // 3 clocks
//             __endasm;
//         }
//         else
//         {
//             IR_TX = 0;
//         }
//     }
//     // restore other interrupt
// }


/* UART */
void uartDisable(void)
{
    setIntUart(false);
}

void uartTxOnly(void)
{
    // setIntGlobal(true);
    setIntUart(true);
    setUartRecv(false);
}

void uartTxRxEnable(void)
{
    setIntUart(true);
    // setIntGlobal(true);
    setUartRecv(true);
}

/* Serial port interrupt routine */
INTERRUPT(serial_isr, SI0_VECTOR)
{
    if (RI)
    {
        RI = 0;             //Clear receive interrupt flag
        rx_byte = SBUF;
        rx_ready = true;
#if (PARITYBIT != NONE_PARITY)
        bit9 = RB8;         //P2.2 show parity bit
#endif
    }
    if (TI)
    {
        TI = 0;             //Clear transmit interrupt flag
        tx_busy = 0;        //Clear transmit busy flag
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
#ifdef UART_BRT
    PCON |= 0x80;
    AUXR |= 0x15;
    BRT = (uint8_t)(TMR_MAX8 - SYSCLK / 16 / BAUD);
#else
    TMOD = 0x20;            //Set Timer1 as 8-bit auto reload mode
    TH1 = TL1 = (uint8_t)(TMR_MAX8 - (SYSCLK / 32 / BAUD)); //Set auto-reload vaule
    TR1 = 1;                //Timer1 start run
#endif
    ES = 1;                 // Enable UART interrupt
    EA = 1;                 // Open master interrupt switch
}

void uart_send_data(const uint8_t dat)
{
    while (tx_busy);        //Wait for the completion of the previous data is sent
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
    tx_busy = 1;
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
    {
        uart_send_data(*s++);   //Send current char and increment string ptr
    }
}

void uart_recv_data(uint8_t* const dat)
{
    if (rx_ready)
    {
        ES = 0;     // Disable UART interrupt
        *dat = rx_byte;
        rx_ready = false;
        ES = 1;     // Enable UART interrupt
    }
}

// void uart_send_hex_string(const uint8_t data)
// {
//     uint8_t data_H = data >> 4;
//     uint8_t data_L = data & 0xf;
//     if (data_H > 9)
//     {
//         uart_send_data('A' + data_H - 10);
//     }
//     else
//     {
//         uart_send_data('0' + data_H);
//     }
//     if (data_L > 9)
//     {
//         uart_send_data('A' + data_L - 10);
//     }
//     else
//     {
//         uart_send_data('0' + data_L);
//     }
// }

#include <stdio.h>
#include "uart.h"
#include "r05d.h"
#include "main.h"

//-----------------------------------------------

typedef enum
{
    NEC,
    MIDEA_R05D
} protocol_t;

/* define variables */
static enum
{
    TASK_NONE,
    TASK_UATR,
    TASK_IR_TX,
    TASK_IR_RX
} task = TASK_IR_TX;

volatile uint16_t timer0;
volatile uint16_t timer1;
volatile uint16_t timer1_count;
uint16_t timer_cycle_on;
uint16_t timer_cycle_off;
volatile signal_t signal;
volatile signal_t signal_next;
volatile bool signal_loaded;

//-----------------------------------------------

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
        if (IR_TX)
            timer0 = timer_cycle_off + TIMER0_OFFSET;
        else
            timer0 = timer_cycle_on + TIMER0_OFFSET;
    }
    else
    {
        IR_TX = 0;
        LED = 1;
    }
}

/* Timer 1 interrupt routine */
INTERRUPT(timer1_isr, TF1_VECTOR)
{
    Timer1Set(timer1);
    if (timer1_count)
        timer1_count--;
    else
    {
        signal = signal_next;
        if (signal == IR_MARK)
            timer0 = timer_cycle_on;
        else
            TR0 = 0;
        timer0_isr();
        signal_loaded = false;
    }
}

/* External interrupt 1 routine */
INTERRUPT(extern1_isr, IE1_VECTOR)
{
    task = TASK_IR_RX;
}

//-----------------------------------------------

#pragma save
#pragma nogcse
void signalGen(signal_t sig_next, uint16_t sig_dur_rem, uint8_t sig_dur_mul)
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

static void uart(void)
{}

static void ir_tx(void)
{
    EX1 = 0;        // Disable external interrupt 1
    ES = 0;         // Disable UART interrupt
    ET0 = 1;        // Enable timer0 interrupt
    ET1 = 1;        // Enable timer1 interrupt
    R05D_tx(0xb2, 0x7b, 0xe0);
    task = TASK_NONE;
    ET0 = 0;        // Disable timer0 interrupt
    ET1 = 0;        // Disable timer1 interrupt
}

static void ir_rx(void)
{
    timer1_count = ~(uint16_t)0;
    signal = IR_MARK;
    EX1 = 0;        // Disable external interrupt 1
    ET1 = 1;        // Enable timer1 interrupt
    ES = 1;         // Enable UART interrupt
    timer1 = TMR16_10US + TIMER1_OFFSET;
    Timer1Set(TMR16_10US + TIMER1_OFFSET + 30);
    while (timer1_count > (TMR_MAX16 - IR_RX_TIMEOUT))
    {
        if (IR_RX != signal)
        {
            uart_send_data(~(uint8_t)(timer1_count >> 8));
            uart_send_data(~(uint8_t)timer1_count);
            signal = IR_RX;
            timer1_count = ~(uint16_t)0;
        }
    }
    task = TASK_NONE;
    ET1 = 0;        // Disable timer1 interrupt
    ES = 0;         // Disable UART interrupt
}

static void init(void)
{
    // Output
    IR_TX = 0;
    LED = 1;

    // Timer
    Timer0Init;
    Timer1Init;
    
    uart_init();

    // Interrupt
    EA = 1;         // open global interrupt switch
}

/* main program */
void main(void)
{
    init();
    uart_send_string("IRremote v0.1\r\n");
    // task loop
    while (1)
    {
        if (task == TASK_NONE)
            EX1 = 1;        // Enable external interrupt 1
        else if (task == TASK_UATR)
            uart();
        else if (task == TASK_IR_TX)
            ir_tx();
        else if (task == TASK_IR_RX)
            ir_rx();
    }
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

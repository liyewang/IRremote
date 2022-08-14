#ifndef _DRIVER_H_
#define _DRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stc12.h>
#include "static_assert.h"

/* define PINS */
#define LED     P3_4
#define IR_TX   P1_0
#define IR_RX   P3_3

#define UART_BRT
#define BAUD            115200//9600
#define NUM_DATA        32

/*Define UART parity mode*/
#define NONE_PARITY     0   //None parity
#define ODD_PARITY      1   //Odd parity
#define EVEN_PARITY     2   //Even parity
#define MARK_PARITY     3   //Mark parity
#define SPACE_PARITY    4   //Space parity

#define PARITYBIT       NONE_PARITY   //Testing even parity

// System clock frequency
#define SYSCLK          22118400ull

#define DIV_1US         1000000
#define DIV_10US        100000
#define DIV_100US       10000
#define DIV_1MS         1000
#define TMR_MAX8        256
#define TMR_MAX16       65536
#define TMR16_1MS       (TMR_MAX16 - SYSCLK / DIV_1MS)  // 1ms 16bit timer counter
#define TMR16_10US      (TMR_MAX16 - SYSCLK / DIV_10US) // 10us 16bit timer counter
#define TMR16_1US       (TMR_MAX16 - SYSCLK / DIV_1US)  // 1us 16bit timer counter
#define TMR8_10US       (TMR_MAX8 - SYSCLK / DIV_10US)  // 10us 8bit timer counter

/* 1T mode */
// Count clocks from .asm
#define TIMER_SET_CLKS  14
#define INT_ENTRY_CLKS  8
#define IF_DET_CLKS     13
#define TIMER0_OFFSET   (TIMER_SET_CLKS + INT_ENTRY_CLKS + IF_DET_CLKS)
#define TIMER1_OFFSET   (TIMER_SET_CLKS + INT_ENTRY_CLKS)

typedef enum
{
    IR_MARK = 0,
    IR_SPACE = 1
} ir_signal_t;

#define IR_TX_ON    1
#define IR_TX_OFF   0
#define IR_RX_MARK  0
#define IR_RX_SPACE 1
#define LED_ON      0
#define LED_OFF     1

#define Timer0Init      \
    {                   \
        AUXR |= 0x80;   \
        TMOD |= 0x01;   \
    }

#define Timer1Init      \
    {                   \
        AUXR |= 0x41;   \
        TMOD |= 0x10;   \
        PT1 = 1;        \
    }

#define Timer0Set(count)                \
    {                                   \
        TL0 = (uint8_t)(count);         \
        TH0 = (uint8_t)((count) >> 8);  \
        TF0 = 0;                        \
        TR0 = 1;                        \
    }

#define Timer1Set(count)                \
    {                                   \
        TL1 = (uint8_t)(count);         \
        TH1 = (uint8_t)((count) >> 8);  \
        TF1 = 0;                        \
        TR1 = 1;                        \
    }

#define Timer0Get   ((uint16_t)TL0 | ((uint16_t)TH0 << 8))
#define Timer1Get   ((uint16_t)TL1 | ((uint16_t)TH1 << 8))

#define get_timer_ms    ((uint16_t)~timer1_count)
/* 
#define send_IR_signal_us(sig_next, sig_dur_us)                                         \
    send_IR_signal(                                                                     \
        (sig_next),                                                                     \
        (TMR_MAX16 - ((((sig_dur_us) * SYSCLK / DIV_1US) * 2 + 1) / 2) % TMR_MAX16),    \
        (((((sig_dur_us) * SYSCLK / DIV_1US) * 2 + 1) / 2) / TMR_MAX16)                 \
        )

#define send_IR_signal_us_lite(sig_next, sig_dur_us)                                    \
    send_IR_signal(                                                                     \
        (sig_next),                                                                     \
        (TMR_MAX16 - ((sig_dur_us) * 22) % TMR_MAX16),                                  \
        (((sig_dur_us) * 22) / TMR_MAX16)                                               \
        )
 */
#define send_IR_symbol_us(sym_mark_us, sym_space_us)                                    \
    send_IR_symbol(                                                                     \
        (TMR_MAX16 - ((((sym_mark_us) * SYSCLK / DIV_1US) * 2 + 1) / 2) % TMR_MAX16),   \
        (((((sym_mark_us) * SYSCLK / DIV_1US) * 2 + 1) / 2) / TMR_MAX16),               \
        (TMR_MAX16 - ((((sym_space_us) * SYSCLK / DIV_1US) * 2 + 1) / 2) % TMR_MAX16),  \
        (((((sym_space_us) * SYSCLK / DIV_1US) * 2 + 1) / 2) / TMR_MAX16)               \
    )

#define send_IR_symbol_us_lite(sym_mark_us, sym_space_us)                               \
    send_IR_symbol(                                                                     \
        (TMR_MAX16 - ((sym_mark_us) * 22) % TMR_MAX16),                                 \
        (((sym_mark_us) * 22) / TMR_MAX16),                                             \
        (TMR_MAX16 - ((sym_space_us) * 22) % TMR_MAX16),                                \
        (((sym_space_us) * 22) / TMR_MAX16)                                             \
    )

#define set_IR_signal_duty(cycle_on, cycle_off) \
    {                                           \
        timer_cycle_on = cycle_on;              \
        timer_cycle_off = cycle_off;            \
    }

#define set_IR_recv_timeout(dur_ms) \
    {                               \
        (recv_timeout = (dur_ms));  \
        STATIC_ASSERT(((dur_ms) < ((uint16_t)~0 / 1000)), "Timeout value must NOT be higher than 65 ms.");  \
    }

#define get_IR_recv_sig ((IR_TX == IR_TX_OFF) && (IR_RX == IR_RX_MARK))

#define get_IE1_flag                \
    (                               \
        IE1_flag                    \
            ? !(IE1_flag = false)   \
            : false                 \
    )

#define uart_recv_ready (rx_ready)

#define setIntGlobal(en)    EA = (bool)(en);
#define setIntExt0(en)      EX0 = (bool)(en);
#define setIntExt1(en)      EX1 = (bool)(en);
#define setIntTimer0(en)    ET0 = (bool)(en);
#define setIntTimer1(en)    ET1 = (bool)(en);
#define setIntUart(en)      ES = (bool)(en);
#define setUartRecv(en)     REN = (bool)(en);

#define ExtMode_Low         0
#define ExtMode_Fall        1
#define setModeExt0(mode)   IT0 = (bool)(mode);
#define setModeExt1(mode)   IT1 = (bool)(mode);

void hw_init(void);

INTERRUPT(timer0_isr, TF0_VECTOR);
INTERRUPT(timer1_isr, TF1_VECTOR);
INTERRUPT(extern1_isr, IE1_VECTOR);
void IRTxEnable(const bool en);
void IRRxEnable(const bool en);
// bool get_IE1_flag(void);
// void set_IR_signal_duty(uint16_t cycle_on, uint16_t cycle_off);
// void send_IR_signal(ir_signal_t sig_next, uint16_t sig_dur_rem, uint8_t sig_dur_mul);
void send_IR_symbol(uint16_t sym_mark_rem, uint8_t sym_mark_mul, uint16_t sym_space_rem, uint8_t sym_space_mul);
void recv_IR_signal(uint16_t* const mark_dur_us, uint16_t* const space_dur_us);
void setTimerMS(const bool en);

INTERRUPT(serial_isr, SI0_VECTOR);
void uart_init(void);
void uartDisable(void);
void uartTxOnly(void);
void uartTxRxEnable(void);
void uart_send_data(const uint8_t dat);
void uart_send_string(const char* const str);
void uart_recv_data(uint8_t* const dat);
// void uart_send_hex_string(const uint8_t data);

extern volatile uint16_t timer1_count;
extern uint16_t timer_cycle_on;
extern uint16_t timer_cycle_off;
extern uint16_t recv_timeout;
extern volatile bool IE1_flag;
extern volatile bool timeout_flag;
extern volatile bool rx_ready;

#endif /* _DRIVER_H_ */

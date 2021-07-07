#ifndef _DRIVER_H_
#define _DRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stc12.h>

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
#define DIV_1MS         1000
#define TMR_MAX8        256
#define TMR_MAX16       65536
#define TMR16_1MS       (TMR_MAX16 - SYSCLK / DIV_1MS)  // 1ms 16bit timer counter
#define TMR16_10US      (TMR_MAX16 - SYSCLK / DIV_10US) // 10us 16bit timer counter
#define TMR8_10US       (TMR_MAX8 - SYSCLK / DIV_10US)  // 10us 8bit timer counter

/* 1T mode */
// Count clocks from .asm
#define TIMER0_SET_CLKS 14
#define TIMER1_SET_CLKS 14
#define INT0_ENTRY_CLKS 8
#define INT1_ENTRY_CLKS 8
#define IF_DET_CLKS     13
#define TIMER0_OFFSET   (TIMER0_SET_CLKS + INT0_ENTRY_CLKS + IF_DET_CLKS)
#define TIMER1_OFFSET   (TIMER1_SET_CLKS + INT1_ENTRY_CLKS)

typedef enum
{
    IR_MARK = 0,
    IR_SPACE = 1
} ir_signal_t;

#define Timer0Init  \
    AUXR |= 0x80;   \
    TMOD |= 0x01;

#define Timer1Init  \
    AUXR |= 0x41;   \
    TMOD |= 0x10;   \
    PT1 = 1;

#define Timer0Set(count)            \
    TL0 = (uint8_t)(count);         \
    TH0 = (uint8_t)((count) >> 8);  \
    TF0 = 0;                        \
    TR0 = 1;

#define Timer1Set(count)            \
    TL1 = (uint8_t)(count);         \
    TH1 = (uint8_t)((count) >> 8);  \
    TF1 = 0;                        \
    TR1 = 1;

#define send_IR_signal_us(sig_next, sig_dur_us)                                         \
    send_IR_signal(                                                                     \
        (sig_next),                                                                     \
        (TMR_MAX16 - ((((sig_dur_us) * SYSCLK / DIV_1US) * 2 + 1) / 2) % TMR_MAX16),    \
        (((((sig_dur_us) * SYSCLK / DIV_1US) * 2 + 1) / 2) / TMR_MAX16)                 \
        );

#define setIntGlobal(en)    EA = (bool)(en);
#define setIntExt0(en)      EX0 = (bool)(en);
#define setIntExt1(en)      EX1 = (bool)(en);
#define setIntTimer0(en)    ET0 = (bool)(en);
#define setIntTimer1(en)    ET1 = (bool)(en);
#define setIntUart(en)      ES = (bool)(en);
#define setUartRecv(en)     REN = (bool)(en);

void hw_init(void);

INTERRUPT(timer0_isr, TF0_VECTOR);
INTERRUPT(timer1_isr, TF1_VECTOR);
INTERRUPT(extern1_isr, IE1_VECTOR);
void IRTxEnable(const bool en);
void IRRxEnable(const bool en);
bool get_IE1_flag(void);
void set_IR_signal_duty(uint16_t cycle_on, uint16_t cycle_off);
void send_IR_signal(ir_signal_t sig_next, uint16_t sig_dur_rem, uint8_t sig_dur_mul);

INTERRUPT(serial_isr, SI0_VECTOR);
void uart_init(void);
void uartDisable(void);
void uartTxOnly(void);
void uartTxRxEnable(void);
void uart_send_data(const uint8_t dat);
void uart_send_string(const char* const str);
bool uart_recv_ready(void);
void uart_recv_data(uint8_t* dat);

#endif /* _DRIVER_H_ */

#ifndef _MAIN_H_
#define _MAIN_H_

#include <stc12.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    IR_MARK = 0,
    IR_SPACE = 1
} signal_t;

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

/* define PINS */
#define LED P3_4
// #define IR_TX P3_4
#define IR_TX P1_0
#define IR_RX P3_3

// System clock frequency
#define SYSCLK  22118400ull

// 1T mode enable
// #define TIMER0_1T
// #define TIMER1_1T
// #define T0x12   1
// #define T1x12   1
// #ifdef MODE1T
// #define T1x12   1
// #else
// #define T1x12   12
// #endif

/* 1T mode */
// Count clocks from .asm
#define TIMER0_SET_CLKS     14
#define TIMER1_SET_CLKS     14
#define INT0_ENTRY_CLKS     8
#define INT1_ENTRY_CLKS     8
#define IF_DET_CLKS         13
#define TIMER0_OFFSET       (TIMER0_SET_CLKS + INT0_ENTRY_CLKS + IF_DET_CLKS)
#define TIMER1_OFFSET       (TIMER1_SET_CLKS + INT1_ENTRY_CLKS)

#define DIV_1US             1000000
#define DIV_10US            100000
#define DIV_1MS             1000
#define TMR_MAX8            256
#define TMR_MAX16           65536
#define TMR16_1MS           (TMR_MAX16 - SYSCLK / DIV_1MS)  // 1ms 16bit timer counter
#define TMR16_10US          (TMR_MAX16 - SYSCLK / DIV_10US) // 10us 16bit timer counter
#define TMR8_10US           (TMR_MAX8 - SYSCLK / DIV_10US)  // 10us 8bit timer counter
#define TMR38K              (TMR_MAX16 - SYSCLK / 38000)    // 38kHz timer counter for carrier wave
#define TMR38K_DUTY_CYCLE   1 / 3                           // 1/3 duty cycle for 38kHz signals
#define TMR38K_CYCLE_ON     (TMR_MAX16 - ((SYSCLK / 38000 * TMR38K_DUTY_CYCLE) * 2 + 1) / 2)    // 65342
#define TMR38K_CYCLE_OFF    (TMR_MAX16 - (SYSCLK / 38000 - (TMR_MAX16 - TMR38K_CYCLE_ON)))      // 65148

#define INT_DELAY_TIMER     (TMR_MAX16 - 100)
#define IR_RX_TIMEOUT       (50 * (DIV_10US / DIV_1MS))

INTERRUPT(timer0_isr, TF0_VECTOR);
INTERRUPT(timer1_isr, TF1_VECTOR);
INTERRUPT(extern1_isr, IE1_VECTOR);
void signalGen(signal_t sig_next, uint16_t sig_dur_rem, uint8_t sig_dur_mul);
static void ir_tx(void);
static void ir_rx(void);

#endif /* _MAIN_H_ */

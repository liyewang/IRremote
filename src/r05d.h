#ifndef _R05D_H_
#define _R05D_H_

#include <stdio.h>
#include "main.h"
#include "uart.h"
#include "driver.h"

#define R05D_NUM_CODE       16

#define R05D_DUR_ERR_MAX    2
#define R05D_DUR_ERR_MIN    1 / 2

#define TMR38K              (TMR_MAX16 - SYSCLK / 38000)    // 38kHz timer counter for carrier wave
#define TMR38K_DUTY_CYCLE   1 / 3                           // 1/3 duty cycle for 38kHz signals
#define TMR38K_CYCLE_ON     (TMR_MAX16 - ((SYSCLK / 38000 * TMR38K_DUTY_CYCLE) * 2 + 1) / 2)    // 65342
#define TMR38K_CYCLE_OFF    (TMR_MAX16 - (SYSCLK / 38000 - (TMR_MAX16 - TMR38K_CYCLE_ON)))      // 65148

#define INT_DELAY_TIMER     (TMR_MAX16 - 100)
#define IR_RX_TIMEOUT       (50 * (DIV_10US / DIV_1MS))

// R05D LEAD symbol
#define R05D_L_1_SYM        IR_MARK
#define R05D_L_1_DUR_US     4400
#define R05D_L_1_CLK        (((R05D_L_1_DUR_US * SYSCLK / DIV_1US) * 2 + 1) / 2)
#define R05D_L_1_TMR_MUL    (R05D_L_1_CLK / TMR_MAX16)//1 //
#define R05D_L_1_TMR_REM    (TMR_MAX16 - R05D_L_1_CLK % TMR_MAX16)//33751 //
#define R05D_L_1_DUR10_MAX  (R05D_L_1_DUR_US * R05D_DUR_ERR_MAX / 10)
#define R05D_L_1_DUR10_MIN  (R05D_L_1_DUR_US * R05D_DUR_ERR_MIN / 10)

#define R05D_L_2_SYM        IR_SPACE
#define R05D_L_2_DUR_US     4400
#define R05D_L_2_CLK        (((R05D_L_2_DUR_US * SYSCLK / DIV_1US) * 2 + 1) / 2)
#define R05D_L_2_TMR_MUL    (R05D_L_2_CLK / TMR_MAX16)//1 //
#define R05D_L_2_TMR_REM    (TMR_MAX16 - R05D_L_2_CLK % TMR_MAX16)//33751 //
#define R05D_L_2_DUR10_MAX  (R05D_L_2_DUR_US * R05D_DUR_ERR_MAX / 10)
#define R05D_L_2_DUR10_MIN  (R05D_L_2_DUR_US * R05D_DUR_ERR_MIN / 10)

// R05D STOP symbol
#define R05D_S_1_SYM        IR_MARK
#define R05D_S_1_DUR_US     540
#define R05D_S_1_CLK        (((R05D_S_1_DUR_US * SYSCLK / DIV_1US) * 2 + 1) / 2)
#define R05D_S_1_TMR_MUL    (R05D_S_1_CLK / TMR_MAX16)//0 //
#define R05D_S_1_TMR_REM    (TMR_MAX16 - R05D_S_1_CLK % TMR_MAX16)//53592 //
#define R05D_S_1_DUR10_MAX  (R05D_S_1_DUR_US * R05D_DUR_ERR_MAX / 10)
#define R05D_S_1_DUR10_MIN  (R05D_S_1_DUR_US * R05D_DUR_ERR_MIN / 10)

#define R05D_S_2_SYM        IR_SPACE
#define R05D_S_2_DUR_US     5220
#define R05D_S_2_CLK        (((R05D_S_2_DUR_US * SYSCLK / DIV_1US) * 2 + 1) / 2)
#define R05D_S_2_TMR_MUL    (R05D_S_2_CLK / TMR_MAX16)//1 //
#define R05D_S_2_TMR_REM    (TMR_MAX16 - R05D_S_2_CLK % TMR_MAX16)//15614 //
#define R05D_S_2_DUR10_MAX  (R05D_S_2_DUR_US * R05D_DUR_ERR_MAX / 10)
#define R05D_S_2_DUR10_MIN  (R05D_S_2_DUR_US * R05D_DUR_ERR_MIN / 10)

// R05D 1 symbol
#define R05D_1_1_SYM        IR_MARK
#define R05D_1_1_DUR_US     540
#define R05D_1_1_CLK        (((R05D_1_1_DUR_US * SYSCLK / DIV_1US) * 2 + 1) / 2)
#define R05D_1_1_TMR_MUL    (R05D_1_1_CLK / TMR_MAX16)//0 //
#define R05D_1_1_TMR_REM    (TMR_MAX16 - R05D_1_1_CLK % TMR_MAX16)//53592 //
#define R05D_1_1_DUR10_MAX  (R05D_1_1_DUR_US * R05D_DUR_ERR_MAX / 10)
#define R05D_1_1_DUR10_MIN  (R05D_1_1_DUR_US * R05D_DUR_ERR_MIN / 10)

#define R05D_1_2_SYM        IR_SPACE
#define R05D_1_2_DUR_US     1620
#define R05D_1_2_CLK        (((R05D_1_2_DUR_US * SYSCLK / DIV_1US) * 2 + 1) / 2)
#define R05D_1_2_TMR_MUL    (R05D_1_2_CLK / TMR_MAX16)//0 //
#define R05D_1_2_TMR_REM    (TMR_MAX16 - R05D_1_2_CLK % TMR_MAX16)//29704 //
#define R05D_1_2_DUR10_MAX  (R05D_1_2_DUR_US * R05D_DUR_ERR_MAX / 10)
#define R05D_1_2_DUR10_MIN  (R05D_1_2_DUR_US * R05D_DUR_ERR_MIN / 10)

// R05D 0 symbol
#define R05D_0_1_SYM        IR_MARK
#define R05D_0_1_DUR_US     540
#define R05D_0_1_CLK        (((R05D_0_1_DUR_US * SYSCLK / DIV_1US) * 2 + 1) / 2)
#define R05D_0_1_TMR_MUL    (R05D_0_1_CLK / TMR_MAX16)//0 //
#define R05D_0_1_TMR_REM    (TMR_MAX16 - R05D_0_1_CLK % TMR_MAX16)//53592 //
#define R05D_0_1_DUR10_MAX  (R05D_0_1_DUR_US * R05D_DUR_ERR_MAX / 10)
#define R05D_0_1_DUR10_MIN  (R05D_0_1_DUR_US * R05D_DUR_ERR_MIN / 10)

#define R05D_0_2_SYM        IR_SPACE
#define R05D_0_2_DUR_US     540
#define R05D_0_2_CLK        (((R05D_0_2_DUR_US * SYSCLK / DIV_1US) * 2 + 1) / 2)
#define R05D_0_2_TMR_MUL    (R05D_0_2_CLK / TMR_MAX16)//0 //
#define R05D_0_2_TMR_REM    (TMR_MAX16 - R05D_0_2_CLK % TMR_MAX16)//53592 //
#define R05D_0_2_DUR10_MAX  (R05D_0_2_DUR_US * R05D_DUR_ERR_MAX / 10)
#define R05D_0_2_DUR10_MIN  (R05D_0_2_DUR_US * R05D_DUR_ERR_MIN / 10)

void R05D_tx(const uint8_t* const data, const uint8_t size);
void R05D_rx(void);

#endif /* _R05D_H_ */

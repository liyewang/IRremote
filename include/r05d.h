#ifndef _R05D_H_
#define _R05D_H_

#define R05D_BLK_SZ         6
#define R05D_BUFF_SZ        (R05D_BLK_SZ * 3)

#define R05D_DUR_ERR_MAX    2
#define R05D_DUR_ERR_MIN    1 / 2

#define TMR38K              (TMR_MAX16 - SYSCLK / 38000)    // 38kHz timer counter for carrier wave
#define TMR38K_DUTY_CYCLE   1 / 3                           // 1/3 duty cycle for 38kHz signals
#define TMR38K_CYCLE_ON     (TMR_MAX16 - ((SYSCLK / 38000 * TMR38K_DUTY_CYCLE) * 2 + 1) / 2)    // 65342
#define TMR38K_CYCLE_OFF    (TMR_MAX16 - (SYSCLK / 38000 - (TMR_MAX16 - TMR38K_CYCLE_ON)))      // 65148

#define INT_DELAY_TIMER     (TMR_MAX16 - 100)
#define RX_SIG_TIMEOUT_MS   30

// R05D LEAD symbol
#define R05D_L_1_SYM        IR_MARK
#define R05D_L_1_DUR_US     4400
#define R05D_L_1_DUR_MAX    (R05D_L_1_DUR_US * R05D_DUR_ERR_MAX)
#define R05D_L_1_DUR_MIN    (R05D_L_1_DUR_US * R05D_DUR_ERR_MIN)

#define R05D_L_2_SYM        IR_SPACE
#define R05D_L_2_DUR_US     4400
#define R05D_L_2_DUR_MAX    (R05D_L_2_DUR_US * R05D_DUR_ERR_MAX)
#define R05D_L_2_DUR_MIN    (R05D_L_2_DUR_US * R05D_DUR_ERR_MIN)

// R05D STOP symbol
#define R05D_S_1_SYM        IR_MARK
#define R05D_S_1_DUR_US     540
#define R05D_S_1_DUR_MAX    (R05D_S_1_DUR_US * R05D_DUR_ERR_MAX)
#define R05D_S_1_DUR_MIN    (R05D_S_1_DUR_US * R05D_DUR_ERR_MIN)

#define R05D_S_2_SYM        IR_SPACE
#define R05D_S_2_DUR_US     5220
#define R05D_S_2_DUR_MAX    (R05D_S_2_DUR_US * R05D_DUR_ERR_MAX)
#define R05D_S_2_DUR_MIN    (R05D_S_2_DUR_US * R05D_DUR_ERR_MIN)

// R05D 1 symbol
#define R05D_1_1_SYM        IR_MARK
#define R05D_1_1_DUR_US     540
#define R05D_1_1_DUR_MAX    (R05D_1_1_DUR_US * R05D_DUR_ERR_MAX)
#define R05D_1_1_DUR_MIN    (R05D_1_1_DUR_US * R05D_DUR_ERR_MIN)

#define R05D_1_2_SYM        IR_SPACE
#define R05D_1_2_DUR_US     1620
#define R05D_1_2_DUR_MAX    (R05D_1_2_DUR_US * R05D_DUR_ERR_MAX)
#define R05D_1_2_DUR_MIN    (R05D_1_2_DUR_US * R05D_DUR_ERR_MIN)

// R05D 0 symbol
#define R05D_0_1_SYM        IR_MARK
#define R05D_0_1_DUR_US     540
#define R05D_0_1_DUR_MAX    (R05D_0_1_DUR_US * R05D_DUR_ERR_MAX)
#define R05D_0_1_DUR_MIN    (R05D_0_1_DUR_US * R05D_DUR_ERR_MIN)

#define R05D_0_2_SYM        IR_SPACE
#define R05D_0_2_DUR_US     540
#define R05D_0_2_DUR_MAX    (R05D_0_2_DUR_US * R05D_DUR_ERR_MAX)
#define R05D_0_2_DUR_MIN    (R05D_0_2_DUR_US * R05D_DUR_ERR_MIN)

#endif /* _R05D_H_ */

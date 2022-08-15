#ifndef _R05D_H_
#define _R05D_H_

#define R05D_BLK_SZ         6
#define R05D_BUF_SZ         (R05D_BLK_SZ * 3)

#define R05D_DUR_ERR_MAX    2
#define R05D_DUR_ERR_MIN    1 / 2
#define R05D_GT_MIN(n,r)    ((r) > (n) * R05D_DUR_ERR_MIN)
#define R05D_LS_MAX(n,r)    ((r) < (n) * R05D_DUR_ERR_MAX)
#define R05D_IN_DUR(n,r)    (R05D_LS_MAX(n,r) && R05D_GT_MIN(n,r))

#define TMR38K              (TMR_MAX16 - SYSCLK / 38000)    // 38kHz timer counter for carrier wave
#define TMR38K_DUTY_CYCLE   1 / 3                           // 1/3 duty cycle for 38kHz signals
#define TMR38K_CYCLE_ON     (TMR_MAX16 - ((SYSCLK / 38000 * TMR38K_DUTY_CYCLE) * 2 + 1) / 2)    // 65342
#define TMR38K_CYCLE_OFF    (TMR_MAX16 - (SYSCLK / 38000 - (TMR_MAX16 - TMR38K_CYCLE_ON)))      // 65148

#define INT_DELAY_TIMER     (TMR_MAX16 - 100)
#define RX_SIG_TIMEOUT_MS   30

// R05D unit duration
#define R05D_U_M_US         540

// R05D LEAD symbol
#define R05D_L_M_US         4400
#define R05D_L_S_US         4400

// R05D STOP symbol
#define R05D_S_M_US         R05D_U_M_US
#define R05D_S_S_US         5220

// R05D 1 symbol
#define R05D_1_M_US         R05D_U_M_US
#define R05D_1_S_US         (R05D_U_M_US * 3)

// R05D 0 symbol
#define R05D_0_M_US         R05D_U_M_US
#define R05D_0_S_US         R05D_U_M_US

void R05D_task(void);

#endif /* _R05D_H_ */

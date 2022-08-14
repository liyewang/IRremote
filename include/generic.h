#ifndef _GENERIC_H_
#define _GENERIC_H_

#define GENE_BLK_SZ         4
#define GENE_BUF_SZ         52

#define GENE_DUR_ERR_MAX    2
#define GENE_DUR_ERR_MIN    1 / 2

#define TMR38K              (TMR_MAX16 - SYSCLK / 38000)    // 38kHz timer counter for carrier wave
#define TMR38K_DUTY_CYCLE   1 / 3                           // 1/3 duty cycle for 38kHz signals
#define TMR38K_CYCLE_ON     (TMR_MAX16 - ((SYSCLK / 38000 * TMR38K_DUTY_CYCLE) * 2 + 1) / 2)    // 65342
#define TMR38K_CYCLE_OFF    (TMR_MAX16 - (SYSCLK / 38000 - (TMR_MAX16 - TMR38K_CYCLE_ON)))      // 65148

#define INT_DELAY_TIMER     (TMR_MAX16 - 100)
#define RX_SIG_TIMEOUT_MS   30

#endif /* _GENERIC_H_ */

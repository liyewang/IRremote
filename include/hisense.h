#ifndef _HISENSE_H_
#define _HISENSE_H_

#define HISE_CUST_SZ        2                   // customer code (0x83, 0x06) size
#define HISE_BLK1_SZ        (HISE_CUST_SZ + 4)  // customer code size + basic data size
#define HISE_BLK2_SZ        8                   // advanced data size including checksum byte
#define HISE_DATA_SZ        (HISE_BLK1_SZ + HISE_BLK2_SZ)
#define HISE_CKS_IDX        (HISE_DATA_SZ - 1)

#define HISE_DUR_ERR_MAX    2
#define HISE_DUR_ERR_MIN    1 / 2
#define HISE_GT_MIN(n,r)    ((r) > (n) * HISE_DUR_ERR_MIN)
#define HISE_LS_MAX(n,r)    ((r) < (n) * HISE_DUR_ERR_MAX)
#define HISE_IN_DUR(n,r)    (HISE_LS_MAX(n,r) && HISE_GT_MIN(n,r))

#define TMR38K              (TMR_MAX16 - SYSCLK / 38000)    // 38kHz timer counter for carrier wave
#define TMR38K_DUTY_CYCLE   1 / 3                           // 1/3 duty cycle for 38kHz signals
#define TMR38K_CYCLE_ON     (TMR_MAX16 - ((SYSCLK / 38000 * TMR38K_DUTY_CYCLE) * 2 + 1) / 2)    // 65342
#define TMR38K_CYCLE_OFF    (TMR_MAX16 - (SYSCLK / 38000 - (TMR_MAX16 - TMR38K_CYCLE_ON)))      // 65148

#define INT_DELAY_TIMER     (TMR_MAX16 - 100)
#define RX_SIG_TIMEOUT_MS   30

// HISE unit duration
#define HISE_U_M_US         560

// HISE LEAD symbol
#define HISE_L_M_US         9000
#define HISE_L_S_US         4500

// HISE STOP symbol
#define HISE_S_M_US         HISE_U_M_US
#define HISE_S_S_US         8000

// HISE 1 symbol
#define HISE_1_M_US         HISE_U_M_US
#define HISE_1_S_US         (HISE_U_M_US * 3)

// HISE 0 symbol
#define HISE_0_M_US         HISE_U_M_US
#define HISE_0_S_US         HISE_U_M_US

#endif /* _HISENSE_H_ */

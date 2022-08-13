#ifndef _HISENSE_H_
#define _HISENSE_H_

#define HISE_BLK1_SZ        (2 + 4) // customer code (0x83, 0x06) size + basic data size
#define HISE_BLK2_SZ        8       // advanced data size including checksum byte
#define HISE_DATA_SZ        (HISE_BLK1_SZ + HISE_BLK2_SZ)
#define HISE_CKS_IDX        (HISE_DATA_SZ - 1)

#define HISE_DUR_ERR_MAX    2
#define HISE_DUR_ERR_MIN    1 / 2

#define TMR38K              (TMR_MAX16 - SYSCLK / 38000)    // 38kHz timer counter for carrier wave
#define TMR38K_DUTY_CYCLE   1 / 3                           // 1/3 duty cycle for 38kHz signals
#define TMR38K_CYCLE_ON     (TMR_MAX16 - ((SYSCLK / 38000 * TMR38K_DUTY_CYCLE) * 2 + 1) / 2)    // 65342
#define TMR38K_CYCLE_OFF    (TMR_MAX16 - (SYSCLK / 38000 - (TMR_MAX16 - TMR38K_CYCLE_ON)))      // 65148

#define INT_DELAY_TIMER     (TMR_MAX16 - 100)
#define RX_SIG_TIMEOUT_MS   30

// HISE LEAD symbol
#define HISE_L_1_SYM        IR_MARK
#define HISE_L_1_DUR_US     9000
#define HISE_L_1_DUR_MAX    (HISE_L_1_DUR_US * HISE_DUR_ERR_MAX)
#define HISE_L_1_DUR_MIN    (HISE_L_1_DUR_US * HISE_DUR_ERR_MIN)

#define HISE_L_2_SYM        IR_SPACE
#define HISE_L_2_DUR_US     4500
#define HISE_L_2_DUR_MAX    (HISE_L_2_DUR_US * HISE_DUR_ERR_MAX)
#define HISE_L_2_DUR_MIN    (HISE_L_2_DUR_US * HISE_DUR_ERR_MIN)

// HISE STOP symbol
#define HISE_S_1_SYM        IR_MARK
#define HISE_S_1_DUR_US     560
#define HISE_S_1_DUR_MAX    (HISE_S_1_DUR_US * HISE_DUR_ERR_MAX)
#define HISE_S_1_DUR_MIN    (HISE_S_1_DUR_US * HISE_DUR_ERR_MIN)

#define HISE_S_2_SYM        IR_SPACE
#define HISE_S_2_DUR_US     8000
#define HISE_S_2_DUR_MAX    (HISE_S_2_DUR_US * HISE_DUR_ERR_MAX)
#define HISE_S_2_DUR_MIN    (HISE_S_2_DUR_US * HISE_DUR_ERR_MIN)

// HISE 1 symbol
#define HISE_1_1_SYM        IR_MARK
#define HISE_1_1_DUR_US     560
#define HISE_1_1_DUR_MAX    (HISE_1_1_DUR_US * HISE_DUR_ERR_MAX)
#define HISE_1_1_DUR_MIN    (HISE_1_1_DUR_US * HISE_DUR_ERR_MIN)

#define HISE_1_2_SYM        IR_SPACE
#define HISE_1_2_DUR_US     1680
#define HISE_1_2_DUR_MAX    (HISE_1_2_DUR_US * HISE_DUR_ERR_MAX)
#define HISE_1_2_DUR_MIN    (HISE_1_2_DUR_US * HISE_DUR_ERR_MIN)

// HISE 0 symbol
#define HISE_0_1_SYM        IR_MARK
#define HISE_0_1_DUR_US     560
#define HISE_0_1_DUR_MAX    (HISE_0_1_DUR_US * HISE_DUR_ERR_MAX)
#define HISE_0_1_DUR_MIN    (HISE_0_1_DUR_US * HISE_DUR_ERR_MIN)

#define HISE_0_2_SYM        IR_SPACE
#define HISE_0_2_DUR_US     560
#define HISE_0_2_DUR_MAX    (HISE_0_2_DUR_US * HISE_DUR_ERR_MAX)
#define HISE_0_2_DUR_MIN    (HISE_0_2_DUR_US * HISE_DUR_ERR_MIN)

#endif /* _HISENSE_H_ */

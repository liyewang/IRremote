#ifndef _R05D_H_
#define _R05D_H_

#include "main.h"

#define R05D_NUM_CODE       16

// R05D LEAD symbol
#define R05D_L_1_SYM        IR_MARK
#define R05D_L_1_DUR_US     4400
#define R05D_L_1_CLK        (((R05D_L_1_DUR_US * SYSCLK / DIV_1US) * 2 + 1) / 2)
#define R05D_L_1_TMR_MUL    (R05D_L_1_CLK / TMR_MAX16)//1 //
#define R05D_L_1_TMR_REM    (TMR_MAX16 - R05D_L_1_CLK % TMR_MAX16)//33751 //
#define R05D_L_2_SYM        IR_SPACE
#define R05D_L_2_DUR_US     4400
#define R05D_L_2_CLK        (((R05D_L_2_DUR_US * SYSCLK / DIV_1US) * 2 + 1) / 2)
#define R05D_L_2_TMR_MUL    (R05D_L_2_CLK / TMR_MAX16)//1 //
#define R05D_L_2_TMR_REM    (TMR_MAX16 - R05D_L_2_CLK % TMR_MAX16)//33751 //

// R05D STOP symbol
#define R05D_S_1_SYM        IR_MARK
#define R05D_S_1_DUR_US     540
#define R05D_S_1_CLK        (((R05D_S_1_DUR_US * SYSCLK / DIV_1US) * 2 + 1) / 2)
#define R05D_S_1_TMR_MUL    (R05D_S_1_CLK / TMR_MAX16)//0 //
#define R05D_S_1_TMR_REM    (TMR_MAX16 - R05D_S_1_CLK % TMR_MAX16)//53592 //
#define R05D_S_2_SYM        IR_SPACE
#define R05D_S_2_DUR_US     5220
#define R05D_S_2_CLK        (((R05D_S_2_DUR_US * SYSCLK / DIV_1US) * 2 + 1) / 2)
#define R05D_S_2_TMR_MUL    (R05D_S_2_CLK / TMR_MAX16)//1 //
#define R05D_S_2_TMR_REM    (TMR_MAX16 - R05D_S_2_CLK % TMR_MAX16)//15614 //

// R05D 1 symbol
#define R05D_1_1_SYM        IR_MARK
#define R05D_1_1_DUR_US     540
#define R05D_1_1_CLK        (((R05D_1_1_DUR_US * SYSCLK / DIV_1US) * 2 + 1) / 2)
#define R05D_1_1_TMR_MUL    (R05D_1_1_CLK / TMR_MAX16)//0 //
#define R05D_1_1_TMR_REM    (TMR_MAX16 - R05D_1_1_CLK % TMR_MAX16)//53592 //
#define R05D_1_2_SYM        IR_SPACE
#define R05D_1_2_DUR_US     1620
#define R05D_1_2_CLK        (((R05D_1_2_DUR_US * SYSCLK / DIV_1US) * 2 + 1) / 2)
#define R05D_1_2_TMR_MUL    (R05D_1_2_CLK / TMR_MAX16)//0 //
#define R05D_1_2_TMR_REM    (TMR_MAX16 - R05D_1_2_CLK % TMR_MAX16)//29704 //

// R05D 0 symbol
#define R05D_0_1_SYM        IR_MARK
#define R05D_0_1_DUR_US     540
#define R05D_0_1_CLK        (((R05D_0_1_DUR_US * SYSCLK / DIV_1US) * 2 + 1) / 2)
#define R05D_0_1_TMR_MUL    (R05D_0_1_CLK / TMR_MAX16)//0 //
#define R05D_0_1_TMR_REM    (TMR_MAX16 - R05D_0_1_CLK % TMR_MAX16)//53592 //
#define R05D_0_2_SYM        IR_SPACE
#define R05D_0_2_DUR_US     540
#define R05D_0_2_CLK        (((R05D_0_2_DUR_US * SYSCLK / DIV_1US) * 2 + 1) / 2)
#define R05D_0_2_TMR_MUL    (R05D_0_2_CLK / TMR_MAX16)//0 //
#define R05D_0_2_TMR_REM    (TMR_MAX16 - R05D_0_2_CLK % TMR_MAX16)//53592 //

void R05D_tx(uint8_t A, uint8_t B, uint8_t C);

#endif /* _R05D_H_ */

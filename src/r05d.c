#include "r05d.h"

extern uint16_t timer_cycle_on;
extern uint16_t timer_cycle_off;

void R05D_tx(uint8_t A, uint8_t B, uint8_t C)
{
    uint8_t cmd = 0;
    timer_cycle_on = TMR38K_CYCLE_ON;
    timer_cycle_off = TMR38K_CYCLE_OFF;
    //  00  01  02  03  04  05  06  07  08  09  10  11  12  13  14  15
    //  L    A  ~A   B  ~B   C  ~C   S   L   A  ~A   B  ~B   C  ~C   S
    for (uint8_t code_idx = 0; code_idx < R05D_NUM_CODE; code_idx++)
    {
        switch (code_idx)
        {
            case 0:
                Timer1Set(INT_DELAY_TIMER);
            case 8:
                signalGen(R05D_L_1_SYM, R05D_L_1_TMR_REM, R05D_L_1_TMR_MUL);
                signalGen(R05D_L_2_SYM, R05D_L_2_TMR_REM, R05D_L_2_TMR_MUL);
                continue;
            case 7:
            case 15:
                signalGen(R05D_S_1_SYM, R05D_S_1_TMR_REM, R05D_S_1_TMR_MUL);
                signalGen(R05D_S_2_SYM, R05D_S_2_TMR_REM, R05D_S_2_TMR_MUL);
                continue;
            case 1:
            case 9:
                cmd = A;
                break;
            case 2:
            case 10:
                cmd = ~A;
                break;
            case 3:
            case 11:
                cmd = B;
                break;
            case 4:
            case 12:
                cmd = ~B;
                break;
            case 5:
            case 13:
                cmd = C;
                break;
            case 6:
            case 14:
                cmd = ~C;
                break;
            default:
                break;
        }
        for (uint8_t bit_idx = 0; bit_idx < 8; bit_idx++)
        {
            if (cmd & (0x80 >> bit_idx))
            {
                signalGen(R05D_1_1_SYM, R05D_1_1_TMR_REM, R05D_1_1_TMR_MUL);
                signalGen(R05D_1_2_SYM, R05D_1_2_TMR_REM, R05D_1_2_TMR_MUL);
            }
            else
            {
                signalGen(R05D_0_1_SYM, R05D_0_1_TMR_REM, R05D_0_1_TMR_MUL);
                signalGen(R05D_0_2_SYM, R05D_0_2_TMR_REM, R05D_0_2_TMR_MUL);
            }
        }
    }
}

#include <stdint.h>
#include <stdbool.h>
#include "r05d.h"

extern uint16_t timer1_count;
extern uint16_t timer1;
extern ir_signal_t signal;

void R05D_tx(const uint8_t* const data, const uint8_t size)
{
    IRTxEnable(true);
    set_IR_signal_duty(TMR38K_CYCLE_ON, TMR38K_CYCLE_OFF);
    Timer1Set(INT_DELAY_TIMER);
    for (uint8_t i = 0; i < 2; i++)
    {
        //  00  01  02  03  04  05  06  07  08  09  10  11  12  13  14  15
        //  L    A  ~A   B  ~B   C  ~C   S   L   A  ~A   B  ~B   C  ~C   S
        // send_IR_signal(R05D_L_1_SYM, R05D_L_1_TMR_REM, R05D_L_1_TMR_MUL);
        // send_IR_signal(R05D_L_2_SYM, R05D_L_2_TMR_REM, R05D_L_2_TMR_MUL);
        send_IR_signal_us(R05D_L_1_SYM, R05D_L_1_DUR_US);
        send_IR_signal_us(R05D_L_2_SYM, R05D_L_2_DUR_US);
        for (uint8_t j = 0; j < size; j++)
        {
            for (uint8_t bit_idx = 0; bit_idx < 8; bit_idx++)
            {
                if (data[j] & (0x80 >> bit_idx))
                {
                    // send_IR_signal(R05D_1_1_SYM, R05D_1_1_TMR_REM, R05D_1_1_TMR_MUL);
                    // send_IR_signal(R05D_1_2_SYM, R05D_1_2_TMR_REM, R05D_1_2_TMR_MUL);
                    send_IR_signal_us(R05D_1_1_SYM, R05D_1_1_DUR_US);
                    send_IR_signal_us(R05D_1_2_SYM, R05D_1_2_DUR_US);
                }
                else
                {
                    // send_IR_signal(R05D_0_1_SYM, R05D_0_1_TMR_REM, R05D_0_1_TMR_MUL);
                    // send_IR_signal(R05D_0_2_SYM, R05D_0_2_TMR_REM, R05D_0_2_TMR_MUL);
                    send_IR_signal_us(R05D_0_1_SYM, R05D_0_1_DUR_US);
                    send_IR_signal_us(R05D_0_2_SYM, R05D_0_2_DUR_US);
                }
            }
        }
        // send_IR_signal(R05D_S_1_SYM, R05D_S_1_TMR_REM, R05D_S_1_TMR_MUL);
        // send_IR_signal(R05D_S_2_SYM, R05D_S_2_TMR_REM, R05D_S_2_TMR_MUL);
        send_IR_signal_us(R05D_S_1_SYM, R05D_S_1_DUR_US);
        send_IR_signal_us(R05D_S_2_SYM, R05D_S_2_DUR_US);
    }
    IRTxEnable(false);
}

// void R05D_tx(uint8_t A, uint8_t B, uint8_t C)
// {
//     uint8_t cmd = 0;
//     set_IR_signal_duty(TMR38K_CYCLE_ON, TMR38K_CYCLE_OFF);
//     //  00  01  02  03  04  05  06  07  08  09  10  11  12  13  14  15
//     //  L    A  ~A   B  ~B   C  ~C   S   L   A  ~A   B  ~B   C  ~C   S
//     for (uint8_t code_idx = 0; code_idx < R05D_NUM_CODE; code_idx++)
//     {
//         switch (code_idx)
//         {
//             case 0:
//                 Timer1Set(INT_DELAY_TIMER);
//             case 8:
//                 send_IR_signal(R05D_L_1_SYM, R05D_L_1_TMR_REM, R05D_L_1_TMR_MUL);
//                 send_IR_signal(R05D_L_2_SYM, R05D_L_2_TMR_REM, R05D_L_2_TMR_MUL);
//                 continue;
//             case 7:
//             case 15:
//                 send_IR_signal(R05D_S_1_SYM, R05D_S_1_TMR_REM, R05D_S_1_TMR_MUL);
//                 send_IR_signal(R05D_S_2_SYM, R05D_S_2_TMR_REM, R05D_S_2_TMR_MUL);
//                 continue;
//             case 1:
//             case 9:
//                 cmd = A;
//                 break;
//             case 2:
//             case 10:
//                 cmd = ~A;
//                 break;
//             case 3:
//             case 11:
//                 cmd = B;
//                 break;
//             case 4:
//             case 12:
//                 cmd = ~B;
//                 break;
//             case 5:
//             case 13:
//                 cmd = C;
//                 break;
//             case 6:
//             case 14:
//                 cmd = ~C;
//                 break;
//             default:
//                 break;
//         }
//         for (uint8_t bit_idx = 0; bit_idx < 8; bit_idx++)
//         {
//             if (cmd & (0x80 >> bit_idx))
//             {
//                 send_IR_signal(R05D_1_1_SYM, R05D_1_1_TMR_REM, R05D_1_1_TMR_MUL);
//                 send_IR_signal(R05D_1_2_SYM, R05D_1_2_TMR_REM, R05D_1_2_TMR_MUL);
//             }
//             else
//             {
//                 send_IR_signal(R05D_0_1_SYM, R05D_0_1_TMR_REM, R05D_0_1_TMR_MUL);
//                 send_IR_signal(R05D_0_2_SYM, R05D_0_2_TMR_REM, R05D_0_2_TMR_MUL);
//             }
//         }
//     }
// }

void R05D_rx(void)
{
    // uint16_t timer;
    // enum
    // {
    //     STATE_INIT,
    //     STATE_DATA,
    //     STATE_FAIL
    // } state = STATE_INIT;
    // enum
    // {
    //     R05D_INIT,
    //     R05D_ERR,
    //     R05D_L,
    //     R05D_S,
    //     R05D_1,
    //     R05D_0
    // } symbol = R05D_ERR;
    // uint8_t data = 0;
    // uint8_t bit_idx = 0;
    // char str[4];
    // timer1_count = ~(uint16_t)0;
    // signal = IR_MARK;
    // EX1 = 0;        // Disable external interrupt 1
    // ET1 = 1;        // Enable timer1 interrupt
    // ES = 1;         // Enable UART interrupt
    // timer1 = TMR16_10US + TIMER1_OFFSET;
    // Timer1Set(TMR16_10US + TIMER1_OFFSET + 30);
    // while (timer1_count > (TMR_MAX16 - IR_RX_TIMEOUT))
    // {
    //     if (IR_RX != signal)
    //     {
    //         timer = ~timer1_count;
    //         timer1_count = ~(uint16_t)0;
    //         signal = IR_RX;
    //         if (IR_RX == IR_MARK)
    //         {
    //             symbol = R05D_INIT;
    //             if (timer > R05D_L_1_DUR10_MIN)
    //             {
    //                 if (timer < R05D_L_1_DUR10_MAX)
    //                     symbol = R05D_L;
    //                 else
    //                     symbol = R05D_ERR;
    //             }
    //             // All the others are 540us, use anyone of them
    //             else if (timer < R05D_S_1_DUR10_MIN || timer > R05D_S_1_DUR10_MAX)
    //                 symbol = R05D_ERR;
    //         }
    //         else
    //         {
    //             if (symbol == R05D_L)
    //                 if (timer < R05D_L_2_DUR10_MIN || timer > R05D_L_2_DUR10_MAX)
    //                     symbol = R05D_ERR;
    //             else if (symbol == R05D_INIT)
    //             {
    //                 if (timer < R05D_0_2_DUR10_MAX)
    //                     if (timer > R05D_0_2_DUR10_MIN)
    //                         symbol = R05D_0;
    //                 else if (timer < R05D_1_2_DUR10_MAX)
    //                     if (timer > R05D_1_2_DUR10_MIN)
    //                         symbol = R05D_1;
    //                 else if (timer < R05D_S_2_DUR10_MAX)
    //                     if (timer > R05D_S_2_DUR10_MIN)
    //                         symbol = R05D_S;
    //             }
    //         }
    //         switch (symbol)
    //         {
    //             case R05D_L:
    //                 if (state == STATE_INIT)
    //                 {
    //                     state = STATE_DATA;
    //                     data = 0;
    //                     bit_idx = 0;
    //                     uart_send_string("\nR05D:\n");
    //                 }
    //                 else
    //                     state = STATE_FAIL;
    //                 break;
    //             case R05D_S:
    //                 if (state == STATE_DATA)
    //                     state = STATE_INIT;
    //                 else
    //                     state = STATE_FAIL;
    //                 break;
    //             case R05D_1:
    //                 data |= (0x80 >> bit_idx);
    //             case R05D_0:
    //                 bit_idx++;
    //                 if (state != STATE_DATA)
    //                     state = STATE_FAIL;
    //                 break;
    //             default:
    //                 state = STATE_FAIL;
    //         }
    //         if (state == STATE_DATA)
    //             if (bit_idx >= 8)
    //             {
    //                 uart_send_data(' ');
    //                 uart_send_hex_string(data);
    //                 uart_send_string(str);
    //                 data = 0;
    //                 bit_idx = 0;
    //             }
    //         else if (state == STATE_INIT)
    //             if (bit_idx)
    //                 state = STATE_FAIL;
    //         if (state == STATE_FAIL)
    //             break;
    //     }
    // }
    // if (state == STATE_INIT)
    //     uart_send_string("\nSuccess.");
    // else
    //     uart_send_string("\nFailed.");
    // ET1 = 0;        // Disable timer1 interrupt
    // ES = 0;         // Disable UART interrupt
}

void Midea_AC(void)
{}
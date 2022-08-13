#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

// #define GENE
// #define R05D
#define HISE

#if defined(GENE)
/* ================================ Generic ================================= */
#include "generic.h"
#define TASK() GENE_task()

static void GENE_tx(const uint8_t* const data, const uint8_t size)
{
    uint8_t data_idx = 0;
    if (size % GENE_BLK_SZ == 0)
    {
        set_IR_signal_duty(TMR38K_CYCLE_ON, TMR38K_CYCLE_OFF);
        Timer1Set(INT_DELAY_TIMER);
        IRTxEnable(true);
        while (data_idx < size / 2)
        {
            send_IR_signal_us_lite(IR_MARK, ((uint16_t*)data)[data_idx++]);
            send_IR_signal_us_lite(IR_SPACE, ((uint16_t*)data)[data_idx++]);
        }
        IRTxEnable(false);
    }
}

static void GENE_rx(void)
{
    uint16_t mark_dur_us;
    uint16_t space_dur_us;
    uint8_t data = 0;
    uint8_t bit_idx = 0;
    set_IR_recv_timeout(RX_SIG_TIMEOUT_MS);
    setTimerMS(true);
    IRRxEnable(true);
    while (get_timer_ms < RX_SIG_TIMEOUT_MS)
    {
        if (get_IE1_flag)
        {
            recv_IR_signal(&mark_dur_us, &space_dur_us);
            uart_send_data((uint8_t)(mark_dur_us >> 8));
            uart_send_data((uint8_t)mark_dur_us);
            uart_send_data((uint8_t)(space_dur_us >> 8));
            uart_send_data((uint8_t)space_dur_us);
        }
    }
    IRRxEnable(false);
    setTimerMS(false);
}

void GENE_task(void)
{
    uint8_t buffer[GENE_BUFF_SZ];
    uint8_t buffer_idx = 0;
    uartTxRxEnable();
    setIntExt1(true);
    if (uart_recv_ready)
    {
        setTimerMS(true);
        while (get_timer_ms < RX_SIG_TIMEOUT_MS)
        {
            if (uart_recv_ready)
            {
                uart_recv_data(buffer + buffer_idx);
                uart_send_data(buffer[buffer_idx++]);
            }
        }
        setTimerMS(false);
        GENE_tx(buffer, buffer_idx);
    }
    if (get_IR_recv_sig)
    {
        GENE_rx();
    }
}
/* ================================ Generic ================================= */

#elif defined(R05D)

/* ================================== R05D ================================== */
#include "r05d.h"
#define TASK() R05D_task()

static void R05D_tx(const uint8_t* const data, const uint8_t size)
{
    if (size % R05D_BLK_SZ == 0)
    {
        set_IR_signal_duty(TMR38K_CYCLE_ON, TMR38K_CYCLE_OFF);
        Timer1Set(INT_DELAY_TIMER);
        IRTxEnable(true);
        for (uint8_t data_idx = 0; data_idx < size; data_idx++)
        {
            if (data_idx % R05D_BLK_SZ == 0)
            {
                send_IR_signal_us(R05D_L_1_SYM, R05D_L_1_DUR_US);
                send_IR_signal_us(R05D_L_2_SYM, R05D_L_2_DUR_US);
            }
            for (uint8_t bit_idx = 0; bit_idx < 8; bit_idx++)
            {
                if (data[data_idx] & (0x80 >> bit_idx))
                {
                    send_IR_signal_us(R05D_1_1_SYM, R05D_1_1_DUR_US);
                    send_IR_signal_us(R05D_1_2_SYM, R05D_1_2_DUR_US);
                }
                else
                {
                    send_IR_signal_us(R05D_0_1_SYM, R05D_0_1_DUR_US);
                    send_IR_signal_us(R05D_0_2_SYM, R05D_0_2_DUR_US);
                }
            }
            if (data_idx % R05D_BLK_SZ == R05D_BLK_SZ - 1)
            {
                send_IR_signal_us(R05D_S_1_SYM, R05D_S_1_DUR_US);
                send_IR_signal_us(R05D_S_2_SYM, R05D_S_2_DUR_US);
            }
        }
        IRTxEnable(false);
    }
}

static void R05D_rx(void)
{
    enum
    {
        STATE_INIT,
        STATE_DATA,
        STATE_STOP,
        STATE_FAIL
    } state = STATE_INIT;
    enum
    {
        R05D_INIT,
        R05D_L,
        R05D_S,
        R05D_1,
        R05D_0
    } symbol;
    uint16_t mark_dur_us;
    uint16_t space_dur_us;
    uint8_t data = 0;
    uint8_t bit_idx = 0;
    set_IR_recv_timeout(RX_SIG_TIMEOUT_MS);
    setTimerMS(true);
    IRRxEnable(true);
    while (get_timer_ms < RX_SIG_TIMEOUT_MS)
    {
        if (get_IE1_flag)
        {
            recv_IR_signal(&mark_dur_us, &space_dur_us);
            if (mark_dur_us > R05D_L_1_DUR_MIN)
            {
                if (mark_dur_us < R05D_L_1_DUR_MAX
                    && space_dur_us > R05D_L_2_DUR_MIN
                    && space_dur_us < R05D_L_2_DUR_MAX)
                {
                    symbol = R05D_L;
                    if (state == STATE_INIT)
                    {
                        state = STATE_DATA;
                    }
                    else
                    {
                        state = STATE_FAIL;
                    }
                }
            }
            else if (mark_dur_us > R05D_S_1_DUR_MIN && mark_dur_us < R05D_S_1_DUR_MAX)
            {
                if (space_dur_us < R05D_0_2_DUR_MAX)
                {
                    if (space_dur_us > R05D_0_2_DUR_MIN)
                    {
                        symbol = R05D_0;
                        bit_idx++;
                        if (state != STATE_DATA)
                        {
                            state = STATE_FAIL;
                        }
                    }
                }
                else if (space_dur_us < R05D_1_2_DUR_MAX)
                {
                    if (space_dur_us > R05D_1_2_DUR_MIN)
                    {
                        symbol = R05D_1;
                        data |= (0x80 >> bit_idx++);
                        if (state != STATE_DATA)
                        {
                            state = STATE_FAIL;
                        }
                    }
                }
                else if (space_dur_us > R05D_S_2_DUR_MIN)
                {
                    symbol = R05D_S;
                    if (state == STATE_DATA && bit_idx == 0)
                    {
                        if (space_dur_us < R05D_S_2_DUR_MAX)
                        {
                            state = STATE_INIT;
                        }
                        else
                        {
                            state = STATE_STOP;
                        }
                    }
                    else
                    {
                        state = STATE_FAIL;
                    }
                }
            }
            else
            {
                state = STATE_FAIL;
            }
            if (state == STATE_DATA && bit_idx == 8)
            {
                uart_send_data(data);
                data = 0;
                bit_idx = 0;
            }
            if (state == STATE_STOP || state == STATE_FAIL)
            {
                break;
            }
        }
    }
    IRRxEnable(false);
    setTimerMS(false);
    if (state == STATE_STOP)
        data = 0x00;
    else
        data = 0xee;
    uart_send_data(data);
}

void R05D_task(void)
{
    uint8_t buffer[R05D_BUFF_SZ];
    uint8_t buffer_idx = 0;
    uartTxRxEnable();
    setIntExt1(true);
    if (uart_recv_ready)
    {
        setTimerMS(true);
        while (get_timer_ms < RX_SIG_TIMEOUT_MS && buffer_idx < HISE_DATA_SZ)
        {
            if (uart_recv_ready)
            {
                uart_recv_data(buffer + buffer_idx);
                uart_send_data(buffer[buffer_idx++]);
            }
        }
        setTimerMS(false);
        R05D_tx(buffer, buffer_idx);
    }
    if (get_IR_recv_sig)
    {
        R05D_rx();
    }
}
/* ================================== R05D ================================== */

#elif defined(HISE)

/* ================================ Hisense ================================= */
#include "hisense.h"
#define TASK() HISE_task()

static void HISE_tx(uint8_t* const data, const uint8_t size)
{
    if (size == HISE_DATA_SZ - 1)
    {
        data[HISE_CKS_IDX] = data[0] ^ data[1];   // pre-remove customer code from checksum
        set_IR_signal_duty(TMR38K_CYCLE_ON, TMR38K_CYCLE_OFF);
        Timer1Set(INT_DELAY_TIMER);
        IRTxEnable(true);
        send_IR_signal_us(HISE_L_1_SYM, HISE_L_1_DUR_US);
        send_IR_signal_us(HISE_L_2_SYM, HISE_L_2_DUR_US);
        for (uint8_t data_idx = 0; data_idx < HISE_DATA_SZ; data_idx++)
        {
            for (uint8_t bit_idx = 0; bit_idx < 8; bit_idx++)
            {
                if (data[data_idx] & (0x01 << bit_idx))
                {
                    send_IR_signal_us(HISE_1_1_SYM, HISE_1_1_DUR_US);
                    send_IR_signal_us(HISE_1_2_SYM, HISE_1_2_DUR_US);
                }
                else
                {
                    send_IR_signal_us(HISE_0_1_SYM, HISE_0_1_DUR_US);
                    send_IR_signal_us(HISE_0_2_SYM, HISE_0_2_DUR_US);
                }
            }
            data[HISE_CKS_IDX] ^= data[data_idx];
            if (data_idx == HISE_BLK1_SZ - 1 || data_idx == HISE_DATA_SZ - 1)
            {
                send_IR_signal_us(HISE_S_1_SYM, HISE_S_1_DUR_US);
                send_IR_signal_us(HISE_S_2_SYM, HISE_S_2_DUR_US);
            }
        }
        IRTxEnable(false);
    }
}

static void HISE_rx(void)
{
    enum
    {
        STATE_INIT,
        STATE_DATA,
        STATE_STOP,
        STATE_FAIL
    } state = STATE_INIT;
    enum
    {
        HISE_INIT,
        HISE_L,
        HISE_S,
        HISE_1,
        HISE_0
    } symbol;
    uint16_t mark_dur_us;
    uint16_t space_dur_us;
    uint8_t data = 0;
    uint8_t bit_idx = 0;
    set_IR_recv_timeout(RX_SIG_TIMEOUT_MS);
    setTimerMS(true);
    IRRxEnable(true);
    while (get_timer_ms < RX_SIG_TIMEOUT_MS)
    {
        if (get_IE1_flag)
        {
            recv_IR_signal(&mark_dur_us, &space_dur_us);
            if (mark_dur_us > HISE_L_1_DUR_MIN)
            {
                if (mark_dur_us < HISE_L_1_DUR_MAX
                    && space_dur_us > HISE_L_2_DUR_MIN
                    && space_dur_us < HISE_L_2_DUR_MAX)
                {
                    symbol = HISE_L;
                    if (state == STATE_INIT)
                    {
                        state = STATE_DATA;
                    }
                    else
                    {
                        state = STATE_FAIL;
                    }
                }
            }
            else if (mark_dur_us > HISE_S_1_DUR_MIN && mark_dur_us < HISE_S_1_DUR_MAX)
            {
                if (space_dur_us < HISE_0_2_DUR_MAX)
                {
                    if (space_dur_us > HISE_0_2_DUR_MIN)
                    {
                        symbol = HISE_0;
                        bit_idx++;
                        if (state != STATE_DATA)
                        {
                            state = STATE_FAIL;
                        }
                    }
                }
                else if (space_dur_us < HISE_1_2_DUR_MAX)
                {
                    if (space_dur_us > HISE_1_2_DUR_MIN)
                    {
                        symbol = HISE_1;
                        data |= (0x01 << bit_idx++);
                        if (state != STATE_DATA)
                        {
                            state = STATE_FAIL;
                        }
                    }
                }
                else if (space_dur_us > HISE_S_2_DUR_MIN)
                {
                    symbol = HISE_S;
                    if (state == STATE_DATA && bit_idx == 0)
                    {
                        if (space_dur_us > HISE_S_2_DUR_MAX)
                        {
                            state = STATE_STOP;
                        }
                    }
                    else
                    {
                        state = STATE_FAIL;
                    }
                }
            }
            else
            {
                state = STATE_FAIL;
            }
            if (state == STATE_DATA && bit_idx == 8)
            {
                uart_send_data(data);
                data = 0;
                bit_idx = 0;
            }
            if (state == STATE_STOP || state == STATE_FAIL)
            {
                break;
            }
        }
    }
    IRRxEnable(false);
    setTimerMS(false);
    if (state == STATE_STOP)
        data = 0x00;
    else
        data = 0xee;
    uart_send_data(data);
}

void HISE_task(void)
{
    uint8_t buffer[HISE_DATA_SZ];
    uint8_t buffer_idx = 0;
    uartTxRxEnable();
    setIntExt1(true);
    if (uart_recv_ready)
    {
        setTimerMS(true);
        while (get_timer_ms < RX_SIG_TIMEOUT_MS && buffer_idx < HISE_DATA_SZ)
        {
            if (uart_recv_ready)
            {
                uart_recv_data(buffer + buffer_idx);
                uart_send_data(buffer[buffer_idx++]);
            }
        }
        setTimerMS(false);
        HISE_tx(buffer, buffer_idx);
    }
    if (get_IR_recv_sig)
    {
        HISE_rx();
    }
}
/* ================================ Hisense ================================= */

#else

/* ================================== None ================================== */
#define TASK()
/* ================================== None ================================== */

#endif

/* main program */
void main(void)
{
    hw_init();
    // main loop
    while (1)
    {
        TASK();
    }
}

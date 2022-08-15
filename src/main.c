#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

// #define GENE
// #define R05D
#define HISE

#if defined(GENE)
/* ================================ Generic ================================= */
#include "generic.h"

static void GENE_tx(const uint8_t* const buf, const uint8_t size)
{
    uint8_t idx = 0;
    if (size % GENE_BLK_SZ == 0)
    {
        set_IR_signal_duty(TMR38K_CYCLE_ON, TMR38K_CYCLE_OFF);
        Timer1Set(INT_DELAY_TIMER);
        IRTxEnable(true);
        while (idx < size / 2)
        {
            send_IR_symbol_us_lite(((uint16_t*)buf)[idx++], ((uint16_t*)buf)[idx++]);
        }
        IRTxEnable(false);
        idx = 0x06; // ACK
    }
    else
    {
        idx = 0x15; // NAK
    }
    uart_send_data(idx);
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

/* main program */
void main(void)
{
    uint8_t buf[GENE_BUF_SZ];
    uint8_t idx;

    hw_init();

    // main loop
    while (1)
    {
        idx = 0;
        uartTxRxEnable();
        setIntExt1(true);
        if (uart_recv_ready)
        {
            setTimerMS(true);
            while (get_timer_ms < RX_SIG_TIMEOUT_MS)
            {
                if (uart_recv_ready && idx < GENE_BUF_SZ)
                {
                    uart_recv_data(buf + idx++);
                }
            }
            setTimerMS(false);
            GENE_tx(buf, idx);
        }
        else if (get_IR_recv_sig)
        {
            GENE_rx();
        }
    }
}
/* ================================ Generic ================================= */

#elif defined(R05D)

/* ================================== R05D ================================== */
#include "r05d.h"

static void R05D_tx(const uint8_t* const buf, const uint8_t size)
{
    uint8_t msg;
    if (size == R05D_BLK_SZ * 2 || size == R05D_BLK_SZ * 3)
    {
        set_IR_signal_duty(TMR38K_CYCLE_ON, TMR38K_CYCLE_OFF);
        Timer1Set(INT_DELAY_TIMER);
        IRTxEnable(true);
        for (uint8_t idx = 0; idx < size; idx++)
        {
            if (idx == 0 || idx == R05D_BLK_SZ || idx == R05D_BLK_SZ * 2)
            {
                send_IR_symbol_us(R05D_L_M_US, R05D_L_S_US);
            }
            for (uint8_t mask = 0x80; mask; mask >>= 1)
            {
                if (buf[idx] & mask)
                {
                    send_IR_symbol_us(R05D_1_M_US, R05D_1_S_US);
                }
                else
                {
                    send_IR_symbol_us(R05D_0_M_US, R05D_0_S_US);
                }
            }
            if (idx == R05D_BLK_SZ - 1 || idx == R05D_BLK_SZ * 2 - 1 || idx == R05D_BLK_SZ * 3 - 1)
            {
                send_IR_symbol_us(R05D_S_M_US, R05D_S_S_US);
            }
        }
        IRTxEnable(false);
        msg = 0x06; // ACK
    }
    else
    {
        msg = 0x15; // NAK
    }
    uart_send_data(msg);
}

static void R05D_rx(uint8_t* const buf)
{
    enum
    {
        STATE_INIT,
        STATE_DATA,
        STATE_STOP,
        STATE_FAIL
    } state = STATE_INIT;
    uint16_t mark_dur_us;
    uint16_t space_dur_us;
    uint8_t mask = 0x80;
    uint8_t data = 0;
    uint8_t idx = 0;
    set_IR_recv_timeout(RX_SIG_TIMEOUT_MS);
    setTimerMS(true);
    IRRxEnable(true);
    while (get_timer_ms < RX_SIG_TIMEOUT_MS)
    {
        if (get_IE1_flag)
        {
            recv_IR_signal(&mark_dur_us, &space_dur_us);
            if (state == STATE_INIT)
            {
                state = STATE_FAIL;
                if (R05D_IN_DUR(R05D_L_M_US, mark_dur_us)
                    && R05D_IN_DUR(R05D_L_S_US, space_dur_us))
                {
                    state = STATE_DATA;
                }
            }
            else if (state == STATE_DATA)
            {
                state = STATE_FAIL;
                if (R05D_IN_DUR(R05D_U_M_US, mark_dur_us))
                {
                    if (R05D_IN_DUR(R05D_0_S_US, space_dur_us))
                    {
                        state = STATE_DATA;
                        mask >>= 1;
                    }
                    else if (R05D_IN_DUR(R05D_1_S_US, space_dur_us))
                    {
                        state = STATE_DATA;
                        data |= mask;
                        mask >>= 1;
                    }
                    if (!mask)
                    {
                        buf[idx++] = data;
                        data = 0;
                        mask = 0x80;
                        if (idx == R05D_BLK_SZ || idx == R05D_BLK_SZ * 2 || idx >= R05D_BUF_SZ)
                        {
                            state = STATE_STOP;
                        }
                    }
                }
            }
            else if (state == STATE_STOP)
            {
                state = STATE_FAIL;
                if (R05D_IN_DUR(R05D_U_M_US, mark_dur_us) && R05D_GT_MIN(R05D_S_S_US, space_dur_us))
                {
                    if (R05D_LS_MAX(R05D_S_S_US, space_dur_us))
                    {
                        if (idx == R05D_BLK_SZ || idx == R05D_BLK_SZ * 2)
                        {
                            state = STATE_INIT;
                        }
                    }
                    else
                    {
                        if (idx == R05D_BLK_SZ * 2 || idx == R05D_BLK_SZ * 3)
                        {
                            for (uint8_t i = 0; i < idx; i++)
                            {
                                uart_send_data(buf[i]);
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    IRRxEnable(false);
    setTimerMS(false);
}

/* main program */
void main(void)
{
    uint8_t buf[R05D_BUF_SZ];
    uint8_t idx;

    hw_init();

    // main loop
    while (1)
    {
        idx = 0;
        uartTxRxEnable();
        setIntExt1(true);
        if (uart_recv_ready)
        {
            setTimerMS(true);
            while (get_timer_ms < RX_SIG_TIMEOUT_MS)
            {
                if (uart_recv_ready && idx < R05D_BUF_SZ)
                {
                    uart_recv_data(buf + idx++);
                }
            }
            setTimerMS(false);
            R05D_tx(buf, idx);
        }
        else if (get_IR_recv_sig)
        {
            R05D_rx(buf);
        }
    }
}
/* ================================== R05D ================================== */

#elif defined(HISE)

/* ================================ Hisense ================================= */
#include "hisense.h"

#define CHECKSUM

static void HISE_tx(const uint8_t* const buf, const uint8_t size)
{
    uint8_t msg;
#ifdef CHECKSUM
    uint8_t data;
    uint8_t checksum = 0;
    if (size == HISE_DATA_SZ - 1)
#else
    if (size == HISE_DATA_SZ)
#endif
    {
        set_IR_signal_duty(TMR38K_CYCLE_ON, TMR38K_CYCLE_OFF);
        Timer1Set(INT_DELAY_TIMER);
        IRTxEnable(true);
        send_IR_symbol_us(HISE_L_M_US, HISE_L_S_US);
        for (uint8_t idx = 0; idx < HISE_DATA_SZ; idx++)
        {
#ifdef CHECKSUM
            if (idx < HISE_CKS_IDX)
            {
                data = buf[idx];
                if (idx > HISE_CUST_SZ - 1)
                {
                    checksum ^= data;
                }
            }
            else
            {
                data = checksum;
            }
    #define DATA    data
#else
    #define DATA    buf[idx]
#endif
            for (uint8_t mask = 0x01; mask; mask <<= 1)
            {
                if (DATA & mask)
                {
                    send_IR_symbol_us(HISE_1_M_US, HISE_1_S_US);
                }
                else
                {
                    send_IR_symbol_us(HISE_0_M_US, HISE_0_S_US);
                }
            }
            if (idx == HISE_BLK1_SZ - 1 || idx == HISE_DATA_SZ - 1)
            {
                send_IR_symbol_us(HISE_S_M_US, HISE_S_S_US);
            }
        }
        IRTxEnable(false);
        msg = 0x06; // ACK
    }
    else
    {
        msg = 0x15; // NAK
    }
    uart_send_data(msg);
}

static void HISE_rx(uint8_t* const buf)
{
    enum
    {
        STATE_INIT,
        STATE_DATA,
        STATE_STOP,
        STATE_FAIL
    } state = STATE_INIT;
    uint16_t mark_dur_us;
    uint16_t space_dur_us;
    uint8_t mask = 0x01;
    uint8_t data = 0;
    uint8_t idx = 0;
#ifdef CHECKSUM
    uint8_t checksum = 0;
#endif
    set_IR_recv_timeout(RX_SIG_TIMEOUT_MS);
    setTimerMS(true);
    IRRxEnable(true);
    while (get_timer_ms < RX_SIG_TIMEOUT_MS)
    {
        if (get_IE1_flag)
        {
            recv_IR_signal(&mark_dur_us, &space_dur_us);
            if (state == STATE_INIT)
            {
                if (HISE_IN_DUR(HISE_L_M_US, mark_dur_us) && HISE_IN_DUR(HISE_L_S_US, space_dur_us))
                {
                    state = STATE_DATA;
                }
                else
                {
                    state = STATE_FAIL;
                }
            }
            else if (state == STATE_DATA)
            {
                state = STATE_FAIL;
                if (HISE_IN_DUR(HISE_U_M_US, mark_dur_us))
                {
                    if (HISE_IN_DUR(HISE_0_S_US, space_dur_us))
                    {
                        state = STATE_DATA;
                        mask <<= 1;
                    }
                    else if (HISE_IN_DUR(HISE_1_S_US, space_dur_us))
                    {
                        state = STATE_DATA;
                        data |= mask;
                        mask <<= 1;
                    }
                    if (!mask)
                    {
#ifdef CHECKSUM
                        if (idx > HISE_CUST_SZ - 1 && idx < HISE_CKS_IDX)
                        {
                            checksum ^= data;
                        }
#endif
                        buf[idx++] = data;
                        data = 0;
                        mask = 0x01;
                        if (idx == HISE_BLK1_SZ || idx >= HISE_DATA_SZ)
                        {
                            state = STATE_STOP;
                        }
                    }
                }
            }
            else if (state == STATE_STOP)
            {
                state = STATE_FAIL;
                if (HISE_IN_DUR(HISE_U_M_US, mark_dur_us) && HISE_GT_MIN(HISE_S_S_US, space_dur_us))
                {
                    if (HISE_LS_MAX(HISE_S_S_US, space_dur_us))
                    {
                        if (idx == HISE_BLK1_SZ)
                        {
                            state = STATE_DATA;
                        }
                    }
                    else
                    {
#ifdef CHECKSUM
    #define DATA_SIZE   HISE_CKS_IDX
    #define VALID_END   ((idx == HISE_DATA_SZ) && (checksum == buf[HISE_CKS_IDX]))
#else
    #define DATA_SIZE   HISE_DATA_SZ
    #define VALID_END   (idx == HISE_DATA_SZ)
#endif
                        if (VALID_END)
                        {
                            for (idx = 0; idx < DATA_SIZE; idx++)
                            {
                                uart_send_data(buf[idx]);
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    IRRxEnable(false);
    setTimerMS(false);
}

/* main program */
void main(void)
{
    uint8_t buf[HISE_DATA_SZ];
    uint8_t idx;

    hw_init();

    // main loop
    while (1)
    {
        idx = 0;
        uartTxRxEnable();
        setIntExt1(true);
        if (uart_recv_ready)
        {
            setTimerMS(true);
            while (get_timer_ms < RX_SIG_TIMEOUT_MS)
            {
                if (uart_recv_ready && idx < HISE_DATA_SZ)
                {
                    uart_recv_data(buf + idx++);
                }
            }
            setTimerMS(false);
            HISE_tx(buf, idx);
        }
        else if (get_IR_recv_sig)
        {
            HISE_rx(buf);
        }
    }
}
/* ================================ Hisense ================================= */

#else

/* ================================== None ================================== */
/* main program */
void main(void)
{
    hw_init();
    // main loop
    while (1)
    {}
}
/* ================================== None ================================== */

#endif

#include "main.h"

#if (FUNC == R05D)

#include "driver.h"
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

void R05D_task(void)
{
    uint8_t buf[R05D_BUF_SZ];
    uint8_t idx;

    while (1)
    {
        uartTxRxEnable();
        setIntExt1(true);
        if (uart_recv_ready)
        {
            idx = 0;
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

#else

// To avoid empty c file warning
void R05D_task(void);

#endif

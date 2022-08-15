#include "main.h"

#if (FUNC == HISE)

#include "driver.h"
#include "hisense.h"

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

void HISE_task(void)
{
    uint8_t buf[HISE_DATA_SZ];
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

#else

// To avoid empty c file warning
void HISE_task(void);

#endif
#include "main.h"

#if (FUNC == GENE)

#include "driver.h"
#include "generic.h"
#include "eeprom.h"

static void GENE_tx(const uint16_t size)
{
    uint16_t idx = 0;
    if (size % 6 == 0)
    {
        set_IR_signal_duty(TMR38K_CYCLE_ON, TMR38K_CYCLE_OFF);
        Timer1Set(INT_DELAY_TIMER);
        IRTxEnable(true);
        while (idx < size)
        {
            send_IR_symbol(
                IapReadByte(IAP_ADDRESS + idx++) << 8 + IapReadByte(IAP_ADDRESS + idx++),
                IapReadByte(IAP_ADDRESS + idx++),
                IapReadByte(IAP_ADDRESS + idx++) << 8 + IapReadByte(IAP_ADDRESS + idx++),
                IapReadByte(IAP_ADDRESS + idx++)
            );
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

void GENE_task(void)
{
    uint8_t dat;
    WORD buf;

    for (uint16_t i = 0; i < EEPROM_SIZE; i++)
    {
        if (IapReadByte(IAP_ADDRESS + i) != 0xff)
        {
            IapEraseSector(IAP_ADDRESS);    //Erase current sector
            break;
        }
    }

    while (1)
    {
        uartTxRxEnable();
        setIntExt1(true);
        if (uart_recv_ready)
        {
            buf = IAP_ADDRESS;
            setTimerMS(true);
            while (get_timer_ms < RX_SIG_TIMEOUT_MS)
            {
                if (uart_recv_ready && buf < EEPROM_SIZE)
                {
                    uart_recv_data(&dat);
                    IapProgramByte(buf++, dat);
                }
            }
            setTimerMS(false);
            GENE_tx(buf - IAP_ADDRESS);
            IapEraseSector(IAP_ADDRESS);    //Erase current sector
        }
        else if (get_IR_recv_sig)
        {
            GENE_rx();
        }
    }
}

#else

// To avoid empty c file warning
void GENE_task(void);

#endif
#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>

#define BUFF_SZ 32

void uart_send_hex_string(const uint8_t data);
void uart_task(void);

#endif /* _UART_H_ */

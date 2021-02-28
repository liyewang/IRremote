#ifndef _UART_H_
#define _UART_H_

#include <stc12.h>
#include <stdint.h>

#define BAUD    115200//9600

/*Define UART parity mode*/
#define NONE_PARITY     0   //None parity
#define ODD_PARITY      1   //Odd parity
#define EVEN_PARITY     2   //Even parity
#define MARK_PARITY     3   //Mark parity
#define SPACE_PARITY    4   //Space parity

#define PARITYBIT   NONE_PARITY   //Testing even parity

INTERRUPT(serial_isr, SI0_VECTOR);
void uart_init(void);
void uart_send_data(const uint8_t dat);
void uart_send_string(const char* const str);

#endif /* _UART_H_ */

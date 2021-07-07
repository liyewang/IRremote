#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    TASK_NONE,
    TASK_UART,
    TASK_IR_TX,
    TASK_IR_RX
} task_t;

static void ir_tx(void);
static void ir_rx(void);

#endif /* _MAIN_H_ */

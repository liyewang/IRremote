#include "driver.h"
#include "r05d.h"

/* main program */
void main(void)
{
    hw_init();
    // main loop
    while (1)
    {
        R05D_task();
    }
}

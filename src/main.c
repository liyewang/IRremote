#include "main.h"
#include "driver.h"

#if (FUNC == GENE)
    #include "generic.h"
    #define TASK(...)   GENE_task(##__VA_ARGS__)
#elif (FUNC == R05D)
    #include "r05d.h"
    #define TTASK(...)  R05D_task(##__VA_ARGS__)
#elif (FUNC == HISE)
    #include "hisense.h"
    #define TASK(...)   HISE_task(##__VA_ARGS__)
#else
    #define TASK(...)
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

#ifndef _MAIN_H_
#define _MAIN_H_

#define GENE    0
#define R05D    1
#define HISE    2

#define FUNC    HISE    // choose a function

#if (FUNC == GENE)
    #define EEPROM
#elif (FUNC == HISE)
    #define CHECKSUM
#endif

#endif /* _MAIN_H_ */

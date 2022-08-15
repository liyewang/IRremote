#ifndef _EEPROM_H_
#define _EEPROM_H_

#define EEPROM_SIZE 2048    // EEPROM size for STC11F02E

typedef unsigned char BYTE;
typedef unsigned int WORD;

//Start address for STC10/11xx EEPROM
#define IAP_ADDRESS 0x0000

void IapIdle(void);
BYTE IapReadByte(WORD addr);
void IapProgramByte(WORD addr, BYTE dat);
void IapEraseSector(WORD addr);

#endif /* _EEPROM_H_ */

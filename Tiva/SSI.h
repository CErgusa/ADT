#include <stdint.h>
#include "tm4c123gh6pm.h"
#define PB7 (*((volatile unsigned long *)0x40005080))
	
void SSI_Init(void);
void SSI_Out(uint8_t code);
//void send_byte(char data);
void send_str(char *buffer);
void SSI_in(void);

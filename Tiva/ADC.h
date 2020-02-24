#include <stdint.h>


#define CELL1_CHANNEL 0x07
#define CELL2_CHANNEL 0x06
#define CELL3_CHANNEL 0x05
#define IR1_CHANNEL   0x04
#define IR2_CHANNEL   0x02
#define IR3_CHANNEL   0x01

void ADC0_Init(void);
void ADC1_Init(void);

uint32_t ADC0_Distance1(void);

uint32_t ADC0_Distance2(void);

uint32_t ADC0_Distance3(void);

uint32_t ADC_Get(int ADC_INDEX, int CHANNEL);

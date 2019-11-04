#include "LCD_SPI.h"
#include "SPI.h"
#include <stdint.h> // uint32_t
#include "tm4c123gh6pm.h" // Registers

char LCD_SPI_InChar(void)
{
  while((SSI1_SR_R & SSI_SR_RNE) == 1);
  return((char)(SSI1_DR_R&SSI_DR_DATA_M));
}


void LCD_SPI_OutChar(char _data)
{
  while((SSI1_SR_R & SSI_SR_TNF) == 1);
  SSI1_DR_R = _data;
}

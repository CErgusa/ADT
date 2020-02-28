#include "utils.h"
#include "UART.h"

#define PI 3.14159f

void hardfault_detect(void)
{
  UART0_OutChar('H');
}

unsigned char value_to_char(unsigned char value)
{
  if (value <= 9)
  {
    return (value+'0');
  }
  else
  {
    return (value+55);
  }
}

void print_hex(int value, int hex_digit)
{
  unsigned char buffer;
  int i;
  
  for (i = 0; i < hex_digit; ++i)
  {
    int shift = (4 * ((hex_digit-1) - i));
    buffer = (value & (0xF << shift)) >> shift;
    UART0_OutChar(value_to_char(buffer));
  }
}


void float_debugging(float value)
{
  float scaled_value = value * 1000.f;
  int int_value = (int)scaled_value;
  
  print_hex(int_value, 4); // should be 4-hex-digit number
}


float degree(float rad)
{
  return rad * (180.f / PI);
}

float radian(float degree)
{
  return degree * (PI / 180.f);
}

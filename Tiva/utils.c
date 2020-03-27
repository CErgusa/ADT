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


void float_angle_debugging_degree(float rad_value)
{
  float scaled_value = (degree(rad_value)) * 10.f;
  int int_value = (int)scaled_value;
  
  print_hex(int_value, 3); // should be 4-hex-digit number
}


float degree(float rad)
{
  return rad * (180.f / PI);
}

float radian(float degree)
{
  return degree * (PI / 180.f);
}

void store_float_in_4char(float *_value, unsigned char *_target)
{
  int IEEE_bits = *((int *)_value);
  
  *(_target)     = (IEEE_bits & 0xFF000000) >> 24;
  *(_target + 1) = (IEEE_bits & 0x00FF0000) >> 16;
  *(_target + 2) = (IEEE_bits & 0x0000FF00) >> 8;
  *(_target + 3) = (IEEE_bits & 0x000000FF) >> 0;
}

void store_int_in_2char(int _value, unsigned char *_target)
{
  (*_target) = ((_value & 0xFF00) >> 8);
  (*(_target+1)) = (_value & 0x00FF);
}

void store_4char_in_float(unsigned char *_start, float *_target)
{
  int value_int = *((int *)_start);
  
  int fixed_endian = (value_int & 0x000000FF) << 24;
    fixed_endian |= ((value_int & 0x0000FF00) << 8);
    fixed_endian |= ((value_int & 0x00FF0000) >> 8);
    fixed_endian |= ((value_int & 0xFF000000) >> 24);

  (*_target) = *(float *)(&fixed_endian);
}

void store_2char_in_int(unsigned char *_start, int *_target)
{
  (*_target) = (((*_start) & 0xFF) << 8);
  (*_target) |= (((*(_start + 1))) & 0xFF);
}

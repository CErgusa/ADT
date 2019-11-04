#include "timer.h"
#include <stdint.h>
#include "tm4c123gh6pm.h"

void Wait_us(int us)
{
  // 1 -> 5us
  int i, j;
  for (i = 0; i < us; ++i)
  {
    for (j = 0; j < 3; ++j)
    {
      // 1 us
    }
  }
}

void Wait_ms(int ms)
{
  int i;
  for (i =0; i < ms; ++i)
  {
    Wait_us(775);
  }
}

void Timer_Init(void)
{
  
  

}

// SysTick.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize the SysTick module, wait at least a
// designated number of clock cycles, and wait approximately a multiple
// of 10 milliseconds using busy wait.  After a power-on-reset, the
// LM4F120 gets its clock from the 16 MHz precision internal oscillator,
// which can vary by +/- 1% at room temperature and +/- 3% across all
// temperature ranges.  If you are using this module, you may need more
// precise timing, so it is assumed that you are using the PLL to set
// the system clock to 50 MHz.  This matters for the function
// SysTick_Wait10ms(), which will wait longer than 10 ms if the clock is
// slower.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the books
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014
   Volume 1, Program 4.7
   
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Program 2.11, Section 2.6

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "SysTick (2).h"

#define NVIC_ST_CTRL_COUNT      0x00010000  // Count flag6
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt enable
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Counter mode
#define NVIC_ST_RELOAD_M        0x00FFFFFF  // Counter load value

void SysTick_80Mhz(void)
{
//  // 1. Using RCC2
//  SYSCTL_RCC2_R |= SYSCTL_RCC2_USERCC2;
//
//  // 2. Bypass PLL while initializing
//  SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2;
//
//  // 3. Write 0x15 on bit[10:6]
//  // clear XTAL bit field first
//  SYSCTL_RCC2_R &= ~SYSCTL_RCC_XTAL_M;
//  // 16Mhz XTAL
//  SYSCTL_RCC2_R |= SYSCTL_RCC_XTAL_16MHZ;
//
//  // 4. Disable OSC source2 -> Select main osc
//  // Clear bit[6:4]
//  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_OSCSRC2_M;
//
//  // 5. Clearing PWRDN to active PLL
//  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_PWRDN2;
//
//  // 6. Set system clock
//  // Use DIV400 Select 400MHz(from PLL)
//  SYSCTL_RCC2_R |= SYSCTL_RCC2_DIV400;
//
//  // 7. Write divisor (value-1) in RCC2[28:22]
//  // 400Mhz / (4+1) = 80Mhz
//  // Clear bits[28:22] first
//  SYSCTL_RCC2_R &= ~(0x1FC00000);
//  // Write 0x04 in bits[28:22]
//  SYSCTL_RCC2_R |= (0x04 << 22);
//
//  // 8. Wait for PLL to lock by polling PLLLRIS
//  while( (SYSCTL_RIS_R & SYSCTL_RIS_PLLLRIS) == 0) {};
//
//  // 9. Set BYPASS to 0, select PLL as the source of system clock
//  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_BYPASS2;
}

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M;  // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it

  // enable SysTick with core clock
  NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC;

  SysTick_80Mhz();
}
// Time delay using busy wait.
// The delay parameter is in units of the core clock. (units of 12.5 nsec for 80 MHz clock)
void SysTick_Wait(uint32_t delay){
  volatile uint32_t elapsedTime;
  uint32_t startTime = NVIC_ST_CURRENT_R;
  do{
    elapsedTime = (startTime-NVIC_ST_CURRENT_R)&0x00FFFFFF;
  }
  while(elapsedTime <= delay);
}
// Time delay using busy wait.
// This assumes 80 MHz system clock.
void SysTick_Wait1us(uint32_t delay){
  uint32_t i;
  for(i=0; i<delay; i++){
    SysTick_Wait(80);  // wait 1us (assumes 80 MHz clock)
  }
}

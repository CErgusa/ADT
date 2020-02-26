#include "FPU.h"
#include <stdint.h>
#include "tm4c123gh6pm.h"

/*
  ; CPACR is located at address 0xE000ED88
  LDR.W R0, =0xE000ED88
  ; Read CPACR
  LDR R1, [R0]
  ; Set bits 20-23 to enable CP10 and CP11 coprocessors
  ORR R1, R1, #(0xF << 20)
  ; Write back the modified value to the CPACR
  STR R1, [R0]; wait for store to complete
  DSB
  ;reset pipeline now the FPU is enabled
  ISB
*/

void FPU_init(void)
{
  // Enable CP11 coprocessor [23:22]
  NVIC_CPAC_R |= NVIC_CPAC_CP11_M;
  
  // Enable CP10 coprocessor [21:20]
  NVIC_CPAC_R |= NVIC_CPAC_CP10_M;
}

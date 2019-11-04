#include "SPI.h"
#include <stdint.h> // uint32_t
#include "tm4c123gh6pm.h" // Registers


// PD0: SSI1 CLK
// PD1: microSD CS
// PD2: SSI1 Rx
// PD3: SSI1 Tx
// PD6: NRF CS <- FOR PROTOTYPE, connected to shitft register


void SSI1_Init(void)
{	
  // 15.4 Initialization and Configuration
  // 1. Enable the SSI module using the RCGCSSI register (see page 346).
  SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R1; // Enable SSI1;

  // 2. Enable the clock to the appropriate GPIO module via the RCGCGPIO register (see page 340).
  //    To find out which GPIO port to enable, refer to Table 23-5 on page 1351.
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3; // Enable Port D
  
  // 3. Set the GPIO AFSEL bits for the appropriate pins (see page 671).
  //    To determine which GPIOs to configure, see Table 23-4 on page 1344.
  GPIO_PORTD_AFSEL_R |= 0x0000000F; // Set PD0~PD3 as Alternative Functions
  
  // 4. Configure the PMCn fields in the GPIOPCTL register to assign the SSI signals
  //    to the appropriate pins. See page 688 and Table 23-5 on page 1351.
  GPIO_PORTD_PCTL_R |= GPIO_PCTL_PD0_SSI1CLK;
  GPIO_PORTD_PCTL_R |= GPIO_PCTL_PD1_SSI1FSS;
  GPIO_PORTD_PCTL_R |= GPIO_PCTL_PD2_SSI1RX;
  GPIO_PORTD_PCTL_R |= GPIO_PCTL_PD3_SSI1TX; // Set PD0~PD3 as Function 2(SSI1)
  
  // 5. Program the GPIODEN register to enable the pin's digital function. 
  //    In addition, the drive strength, drain select and pull-up/pull-down functions must be configured. 
  //    Refer to “General-Purpose Input/Outputs (GPIOs)” on page 649 for more information.
  GPIO_PORTD_DEN_R |= 0x00000CF; // enable digital I/O on PD07-6, PD3-0
  
  // PD7, PD6, PD3, PD1, PD0 are for output, PD2 is input
  GPIO_PORTD_DIR_R |= 0xCB;
  
  // Clock(PD0), Outputs(PD1, PD3): Pull-down register
  //GPIO_PORTD_PUR_R |= 0xB; // DOES NOT WORK
    
  // For each of the frame formats, the SSI is configured using the following steps:
  // 1. Ensure that the SSE bit in the SSICR1 register is clear before making any configuration changes.
  SSI1_CR1_R &= ~(SSI_CR1_SSE); // Clear SSE bit
  
  // 2. Select whether the SSI is a master or slave:
  // a. For master operations, set the SSICR1 register to 0x00000000
  SSI1_CR1_R = 0x00000000; // Set SSI1 as Master
  
  // 3. Configure the SSI clock source by writing to the SSICC register.
  SSI1_CC_R = 0x00000000; // Use system clock: 16Mhz
  
  // 4. Configure the clock prescale divisor by writing the SSICPSR register.
  SSI1_CPSR_R = 0x28; // Use CPSDVSR = 40
  
  // SSInClk = SysClk / (CPSDVSR * (1 + SCR)) = 16M / (40 * (1+0)) = 1M 
 
  
  // 5. Write the SSICR0 register with the following configuration:
  //    ¦ Serial clock rate (SCR)
  //    ¦ Desired clock phase/polarity, if using Freescale SPI mode (SPH and SPO)
  //    ¦ The protocol mode: Freescale SPI, TI SSF, MICROWIRE (FRF)
  //    ¦ The data size (DSS)
  SSI1_CR0_R |= 0x00000000;       // SCR = 0
  SSI1_CR0_R &= ~(SSI_CR0_SPH);   // SPH = 0 ,Clock Phase(set to 0): Data is captured on the first clock edge transition
  SSI1_CR0_R &= ~(SSI_CR0_SPO);   // SPO = 0 ,Clock Polarity(set to 0): Steady State LOW
  SSI1_CR0_R |= SSI_CR0_FRF_MOTO; // The protocol mode: Freescale SPI pro
  SSI1_CR0_R |= SSI_CR0_DSS_8;    // The data size: 8bits
  
  // 7. Enable the SSI by setting the SSE bit in the SSICR1 register.
  SSI1_CR1_R |= SSI_CR1_SSE;
  
  // Check the clk freq and data tx
  //while (1)
  //{
    //SSI1_Write(0x0A);
    //SSI1_Write(0x0B);
    //SSI1_Write(0x0C);
    //SSI1_Write(0x0D);
    //SSI1_Write(0x0E);
    //SSI1_Write(0x0F);
  //}
}

void SSI1_Write(unsigned char _data)
{
 	unsigned short volatile rcvdata;
	while((SSI1_SR_R&SSI_SR_TNF)==0){};
	//DC = DC_DATA;
	SSI1_DR_R = _data;
	while((SSI1_SR_R&SSI_SR_RNE)==0){};
	rcvdata = SSI1_DR_R;
}

void SSI1_WriteByte()
{
  
}


unsigned char SSI1_Read(void)
{
  while((SSI1_SR_R&SSI_SR_TNF)==0){};    // wait until room in FIFO
	SSI1_DR_R = 0xFF;                      // data out, garbage
	while((SSI1_SR_R&SSI_SR_RNE)==0){};    // wait until response
	return (unsigned char) SSI1_DR_R;      // read received data
}


void SSI1_CopyDataToBuffer(unsigned char *_buffer)
{
  *_buffer = SSI1_Read();
}



//********SSI_Init*****************
// inputs:  none
// outputs: none
// System clock is 16MHz: SSICLK 1MHz
// PA2: CLK
// PA3: Fss
// PA4: Rx
// PA5: Tx
void SSI0_Init(void)
{
  SYSCTL_RCGCSSI_R |= 0x01;               // activate SSI0
  SYSCTL_RCGCGPIO_R |= 0x01;              // activate port A
  while((SYSCTL_PRGPIO_R&0x01) == 0){};   // ready?
    
  GPIO_PORTA_AFSEL_R |= 0x3C;             // enable alt funct on PA2,3,4,5
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFF0000FF)+0x00222200;
  GPIO_PORTA_AMSEL_R = 0;                 // disable analog functionality on PA
  GPIO_PORTA_DEN_R |= 0x3C;               // enable digital I/O on PA2,3, 4, 5
  SSI0_CR1_R = 0x00000000;                // disable SSI, master mode
    
  SSI0_CPSR_R = 0x10;                     // To make 1MHz
  // SSInClk = SysClk / (CPSDVSR * (1 + SCR)) = 16M / (16 * (1+0)) = 1M
    
  SSI0_CR0_R &= ~(0x0000FFF0);            // SCR = 0, SPH = 0, SPO = 0 Freescale
  SSI0_CR0_R = (SSI0_CR0_R&~0x0F)+0x07;   // 8-bit data
  SSI0_CR1_R |= 0x00000002;               // enable SSI
}

//********SSI_Out*****************
// Send data to 74HC595 8-bit port
// inputs:  output (0 to 255)
// outputs: none
void SSI0_Out(unsigned char code)
{
  while((SSI0_SR_R&0x02)==0){}; // wait until room in FIFO
  SSI0_DR_R = code;             // data out
}


#include "ADC.h"
#include "tm4c123gh6pm.h"

// We will use 5 pins of Port D
// Cell3 -> PD0(AIN7) -> ADC0
// Cell2 -> PD1(AIN6) -> ADC0
// Cell1 -> PD2(AIN5) -> ADC0
// IR1   -> PD3(AIN4) -> ADC1
//          PD6 -> GDL2
// PD7: This pin is locked as GPIO by default
//      should unlock the pin in the GPIOLOCK register and uncommit it by setting the GPIOCR register
void PortD_Init(void)
{
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGC2_GPIOD; // 1) activate clock for Port D
 
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R3) == 0){};
	
  // Set input/output
  // input: clear [3:0]
  // output: set [6]
	GPIO_PORTD_DIR_R &= ~0x0F;
  GPIO_PORTD_DIR_R |= 0x40;
 
	// Alt function
  // Enable alt function: set [3:0]
  // Disable alt funcitno: clear [6]
  GPIO_PORTD_AFSEL_R |= 0x0F;
  GPIO_PORTD_AFSEL_R &= ~(0x40);
  
  //GPIOPCTL GPIOADCCTL
  
    
  // For GPIO pins,
  // Drive Strength: 8mA
  // Clear DR2R bit
  GPIO_PORTD_DR2R_R &= ~0xFF;
  
  // set [6]
  GPIO_PORTD_DR8R_R |= 0x40;

  // Disable Open Drain
  // clear [6]
  GPIO_PORTD_ODR_R &= ~(0x40);

  // Disable pull-up
  // clear [6]
  GPIO_PORTD_PUR_R &= ~(0x40);

  // Enable pull-down
  // set [6]
  GPIO_PORTD_PDR_R |= 0x40;
  

    
  // Digital I/O
  // Enable digital I/O: set [6]
  // Disable digital I/O: clear [3:0]
	GPIO_PORTD_DEN_R |= 0x40; 
  GPIO_PORTD_DEN_R &= ~0x0F;
	
  // Analog function
  // Enable analog function: set [3:0]
  // Disable analog function: clear [6]
  GPIO_PORTD_AMSEL_R |= 0x0F;
  GPIO_PORTD_AMSEL_R &= ~(0x40);
}

// We will use 2 pins of Port E
// IR2 -> PE1(AIN2) -> ADC1
// IR3 -> PE2(AIN1) -> ADC1
void PortE_Init(void)
{
	SYSCTL_RCGCGPIO_R |= 0x10; // 1) activate clock for Port E
  while((SYSCTL_PRGPIO_R&0x10) == 0){};

	GPIO_PORTE_DIR_R &= ~0x06;  // 2) make input
	
  GPIO_PORTE_AFSEL_R |= 0x06; // 3) enable alternate function
                              //    PE1:AIN2, PE2: AIN1
	GPIO_PORTE_DEN_R &= ~0x06;  // 4) disable digital I/O
	
  GPIO_PORTE_AMSEL_R |= 0x06; // 5) enable analog function
}

// ADC0 for reading the voltage of battery cell
// Cell3 -> PD0(AIN7) -> ADC0
// Cell2 -> PD1(AIN6) -> ADC0
// Cell1 -> PD2(AIN5) -> ADC0
void ADC0_Init(void)
{
	SYSCTL_RCGCADC_R |= 0x01; // 6) activate ADC0
  
  // Add a small delay to allow the ADC to activate
  while((SYSCTL_PRADC_R&SYSCTL_PRADC_R0) == 0){};
  
  PortD_Init();

  ADC0_PC_R = 0x01;         // 7) configure for 125K
	
  ADC0_SSPRI_R = 0x0123;    // 8) Sequencer 3 is highest priority
  
  ADC0_ACTSS_R &= ~0x0008;  // 9) disable sample sequencer 3
	
  ADC0_EMUX_R &= ~0xF000;   // 10) seq3 is software trigger
    
  ADC0_SSMUX3_R &= ~0x000F; // 11) clear SS3 field

	ADC0_SSCTL3_R = 0x0006;   // 12) no TS0 D0, yes IE0 END0
	
  ADC0_IM_R &= ~0x0008;     // 13) disable SS3 interrupts
	
  ADC0_ACTSS_R |= 0x0008;   // 14) enable sample sequencer 3
}

// ADC1 for reading the IR sensor value
// IR1 -> PD3(AIN4) -> ADC1
// IR2 -> PE1(AIN2) -> ADC1
// IR3 -> PE2(AIN1) -> ADC1
void ADC1_Init(void)
{
	SYSCTL_RCGCADC_R |= 0x02; // 6) activate ADC0
  
  // Add a small delay to allow the ADC to activate
  while((SYSCTL_PRADC_R&SYSCTL_PRADC_R1) == 0){};
  
  PortE_Init();

  ADC1_PC_R = 0x01;         // 7) configure for 125K
	
  ADC1_SSPRI_R = 0x0123;    // 8) Sequencer 3 is highest priority
  
  ADC1_ACTSS_R &= ~0x0008;  // 9) disable sample sequencer 3
	
  ADC1_EMUX_R &= ~0xF000;   // 10) seq3 is software trigger

	ADC1_SSCTL3_R = 0x0006;   // 12) no TS0 D0, yes IE0 END0
	
  ADC1_IM_R &= ~0x0008;     // 13) disable SS3 interrupts
	
  ADC1_ACTSS_R |= 0x0008;   // 14) enable sample sequencer 3
}

// Cell3 -> PD0(AIN7) -> ADC0
// Cell2 -> PD1(AIN6) -> ADC0
// Cell1 -> PD2(AIN5) -> ADC0
// IR1   -> PD3(AIN4) -> ADC1
// IR2   -> PE1(AIN2) -> ADC1
// IR3   -> PE2(AIN1) -> ADC1
uint32_t ADC_Get(int ADC_INDEX, int CHANNEL)
{
  uint32_t result;
  switch (ADC_INDEX)
  {
    case 0: // ADC0
    {
      ADC0_SSMUX3_R &= ~0x000F; // 11) clear SS3 field
      
      switch (CHANNEL)
      {
        case CELL1_CHANNEL: // Cell1 -> PD2(AIN5) -> ADC0
        {
          ADC0_SSMUX3_R += 5; // set channel AIN5(PD2)
          break;
        }
        case CELL2_CHANNEL: // Cell2 -> PD1(AIN6) -> ADC0
        {
          ADC0_SSMUX3_R += 6; // set channel AIN6(PD1)
          break;
        }
        case CELL3_CHANNEL: // Cell3 -> PD0(AIN7) -> ADC0
        {
          ADC0_SSMUX3_R += 7; // set channel AIN7(PD0)
          break;
        }
        default: break;
      }
      ADC0_PSSI_R = 0x0008;          // 1) initiate SS3
      while((ADC0_RIS_R&0x08)==0){}; // 2) wait for conversion done
      result = ADC0_SSFIFO3_R&0xFFF; // 3) read 12-bit result
      ADC0_ISC_R = 0x0008;           // 4) acknowledge completion
      break;
    }
    case 1: // ADC1
    {
      ADC1_SSMUX3_R &= ~0x000F; // 11) clear SS3 field
      switch (CHANNEL)
      {
        case IR3_CHANNEL: // IR3 -> PE2(AIN1) -> ADC1
        {
          ADC1_SSMUX3_R += 1; // set channel AIN1(PE2)
          break;
        }
        case IR2_CHANNEL: // IR2 -> PE1(AIN2) -> ADC1
        {
          ADC1_SSMUX3_R += 2; // set channel AIN2(PE1)
          break;
        }
        case IR1_CHANNEL: // IR1 -> PD3(AIN4) -> ADC1
        {
          ADC1_SSMUX3_R += 4; // set channel AIN4(PD3)
          break;
        }
        default: break;
      }
      ADC1_PSSI_R = 0x0008;          // 1) initiate SS3
      while((ADC1_RIS_R&0x08)==0){}; // 2) wait for conversion done
      result = ADC1_SSFIFO3_R&0xFFF; // 3) read 12-bit result
      ADC1_ISC_R = 0x0008;           // 4) acknowledge completion
      break;
    }
    default: break;
  }  
  return result; 
}


// pg. 801 setting channel details
uint32_t ADC0_Distance1(void)
{ 
	// PD0 / AIN7
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0) // 11) clear SS3 field
	+ 5; // set channel
	
	uint32_t result;
	ADC0_PSSI_R = 0x0008; // 1) initiate SS3
	while((ADC0_RIS_R&0x08)==0){}; // 2) wait for conversion done
	result = ADC0_SSFIFO3_R&0xFFF; // 3) read 12-bit result
	ADC0_ISC_R = 0x0008; // 4) acknowledge completion
	return result;
}

uint32_t ADC0_Distance2(void)
{ 
	// PD1 / AIN6
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0) // 11) clear SS3 field
	+ 6; // set channel
	
	uint32_t result;
	ADC0_PSSI_R = 0x0008; // 1) initiate SS3
	while((ADC0_RIS_R&0x08)==0){}; // 2) wait for conversion done
	result = ADC0_SSFIFO3_R&0xFFF; // 3) read 12-bit result
	ADC0_ISC_R = 0x0008; // 4) acknowledge completion
	return result;
}

uint32_t ADC0_Distance3(void)
{
	// PD2 / AIN5
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0) // 11) clear SS3 field
	+ 7; // set channel
	
	uint32_t result;
	ADC0_PSSI_R = 0x0008; // 1) initiate SS3
	while((ADC0_RIS_R&0x08)==0){}; // 2) wait for conversion done
	result = ADC0_SSFIFO3_R&0xFFF; // 3) read 12-bit result
	ADC0_ISC_R = 0x0008; // 4) acknowledge completion
	return result;
}

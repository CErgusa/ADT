#include "adc.h"
#include "UART1.h"

void ADC0_Init(void)
{
	SYSCTL_RCGCGPIO_R |= 0x10; // 1) activate clock for Port E
 
	while((SYSCTL_PRGPIO_R&0x10) == 0){};
		UART_OutChar('1');

	// Port D (unlock 0-3)
	GPIO_PORTE_DIR_R &= ~0x0F; // 2) make input
	GPIO_PORTE_AFSEL_R |= 0x0F; // 3) enable alternate function
	GPIO_PORTE_DEN_R &= ~0x0F; // 4) disable digital I/O
	GPIO_PORTE_AMSEL_R |= 0x0F; // 5) enable analog function
		
		
	SYSCTL_RCGCADC_R |= 0x01; // 6) activate ADC0
		UART_OutChar('2');
	ADC0_PC_R = 0x01; // 7) configure for 125K
	ADC0_SSPRI_R = 0x0123; // 8) Sequencer 3 is highest priority
	ADC0_ACTSS_R &= ~0x0008; // 9) disable sample sequencer 3
	ADC0_EMUX_R &= ~0xF000; // 10) seq3 is software trigger

	ADC0_SSCTL3_R = 0x0006; // 12) no TS0 D0, yes IE0 END0
	ADC0_IM_R &= ~0x0008; // 13) disable SS3 interrupts
	ADC0_ACTSS_R |= 0x0008; // 14) enable sample sequencer 3
		UART_OutChar('3');
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

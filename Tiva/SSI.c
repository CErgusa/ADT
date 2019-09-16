#include "SSI.h"
#include "adc.h"

void SSI_Init(void)
{
	SYSCTL_RCGCSSI_R|=(1<<2); //selecting SSI2 module
	SYSCTL_RCGC2_R|=(1<<1);   //providing clock to PORTB
	GPIO_PORTB_AFSEL_R|=(1<<4)|(1<<5)|(1<<6)|(1<<7);//selecting alternative fuctions
	GPIO_PORTB_PCTL_R=0x22220000;//selecting SSI as alternative function
	GPIO_PORTB_DEN_R|=(1<<4)|(1<<5)|(1<<6)|(1<<7);//enabling digital mode for PORTB 4,5,6,7
	GPIO_PORTB_DIR_R |= (1<<7) | (1<<4) | (1<<5);
	GPIO_PORTB_PUR_R|=(1<<4)|(1<<5)|(1<<6)|(1<<7);//selecting pull ups for 4,5,6,7
	SSI2_CR1_R=0x0;          //disabling SSI module for settings
	SSI2_CC_R=0;           //using main system clock
	SSI2_CPSR_R=32;        //selecting divisor 64 for SSI clock
	SSI2_CR0_R=0x7;        //freescale mode, 8 bit data, steady clock low 
	SSI2_CR1_R = 0x4;
	SSI2_CR1_R|=(1<<1);    //enabling SSI
}

void send_byte(char data){
		SSI2_DR_R=data; //putting the byte to send from SSI
	while((SSI2_SR_R&(1<<0))==0); //waiting for transmission to be done
}
 
void send_str(char *buffer){ //function for sending each byte of string one by one
  while(*buffer!=0){ 
  send_byte(*buffer);
		buffer++;
	}
}
void SSI_in(void){
	// while CS isn't activated
  while(PB7==0){
		int distance1 = ADC0_Distance1();
		int distance2 = ADC0_Distance2();
		int distance3 = ADC0_Distance3();
		
		send_byte(ADC0_Distance1());
		send_byte(ADC0_Distance2());
		send_byte(ADC0_Distance3());
		
	}; 
}


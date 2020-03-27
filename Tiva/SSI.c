#include "SSI.h"
#include <stdint.h>
#include "tm4c123gh6pm.h"

// SSI0
// Tiva        Rpi
// PA2: CLK  <-- BCM11 (SPI0 SCLK)
// PA3: CS   <-- BCM8  (SPI0 CE0)
// PA4: Rx:MOSI <-- BCM10 (SPI0 MOSI)
// PA5: Tx:MISO --> BCM9  (SPI0 MISO)
void SSI_Init(void)
{
	SYSCTL_RCGCSSI_R|= SYSCTL_RCGCSSI_R0; // selecting SSI0 module
	SYSCTL_RCGC2_R |= SYSCTL_RCGCGPIO_R0;   // providing clock to PORTA
	GPIO_PORTA_AFSEL_R |= (1<<2)|(1<<3)|(1<<4)|(1<<5); // selecting alternative fuctions
	GPIO_PORTA_PCTL_R|=0x222200; // selecting PA2-5 SSI as alternative function
	GPIO_PORTA_DEN_R|=(1<<2)|(1<<3)|(1<<4)|(1<<5); // enabling digital mode for PA2-5
	GPIO_PORTA_DIR_R &= ~((1<<2) | (1<<3) | (1<<4)); // Input: CLK(2), CS(3), MOSI(4)
  GPIO_PORTA_DIR_R |= (1<<5);  // Output: MISO
	GPIO_PORTA_PUR_R |= (1<<2)|(1<<3)|(1<<4)|(1<<5); //selecting pull ups for PA2-5
	SSI0_CR1_R = 0x0;        //disabling SSI module for settings
	SSI0_CC_R = 0;           //using main system clock
	SSI0_CPSR_R = 32;        //selecting divisor 64 for SSI clock

	SSI0_CR0_R = 0x7;        //freescale mode, 8 bit data, steady clock low
	SSI0_CR1_R = 0x4;      // Slave, output enabled
	SSI0_CR1_R |= (1<<1);    //enabling SSI
}

void SSI_send_byte(char data)
{
  while((SSI0_SR_R&SSI_SR_TNF)==0){};  // wait until room in Transmit FIFO
  SSI0_DR_R=data; //putting the byte to send from SSI
	while((SSI0_SR_R&(1<<0))==0); //waiting for transmission to be done
}
 
void send_str(char *buffer){ //function for sending each byte of string one by one
  while(*buffer!=0){ 
  SSI_send_byte(*buffer);
		buffer++;
	}
}

unsigned char SSI_read_byte(unsigned char data)
{
  while((SSI0_SR_R&SSI_SR_TNF)==0){};  // wait until room in Transmit FIFO
	SSI0_DR_R = data;                    // data out, garbage
	while((SSI0_SR_R&SSI_SR_RNE)==0){};    // wait until response Receiving FIFO is Full
	return (unsigned char) SSI0_DR_R;      // read received data
}

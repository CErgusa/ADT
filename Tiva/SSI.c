#include "SSI.h"
#include "adc.h"
#define SSI0_CE_ON 0x0
#define SSI0_CE_OFF 0x1

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

unsigned char SSI_Read(unsigned char data)
{
  while((SSI0_SR_R&SSI_SR_TNF)==0){};  // wait until room in Transmit FIFO
	SSI0_DR_R = data;                    // data out, garbage
	while((SSI0_SR_R&SSI_SR_RNE)==0){};    // wait until response Receiving FIFO is Full
	return (unsigned char) SSI0_DR_R;      // read received data
}


void SSI_in(void){

  uint32_t IR_Raw[3];
  uint32_t CELL_Raw[3];
  
  unsigned char IR_CELL_MSB[6];

  IR_Raw[0] = ADC_Get(1, IR1_CHANNEL);
  IR_Raw[1] = ADC_Get(1, IR2_CHANNEL);
  IR_Raw[2] = ADC_Get(1, IR3_CHANNEL);
  
  CELL_Raw[0] = ADC_Get(0, CELL1_CHANNEL);
  CELL_Raw[1] = ADC_Get(0, CELL2_CHANNEL);
  CELL_Raw[2] = ADC_Get(0, CELL3_CHANNEL); 
  
  // Only MSB 8bits
  IR_CELL_MSB[0] = IR_Raw[0] >> 4;
  IR_CELL_MSB[1] = IR_Raw[1] >> 4;
  IR_CELL_MSB[2] = IR_Raw[2] >> 4;
  
  IR_CELL_MSB[3] = CELL_Raw[0] >> 4;
  IR_CELL_MSB[4] = CELL_Raw[1] >> 4;
  IR_CELL_MSB[5] = CELL_Raw[2] >> 4;
  
  int SSI_CE_PA3 = (GPIO_PORTA_DATA_R & 0x08);
  int count = 0;
  if (SSI_CE_PA3 == SSI0_CE_ON)
  {
    // Send IR1 data first
    SSI_send_byte(IR_CELL_MSB[count++]);
    while(count < 6) // Not done until IR2, IR3, CELL1, CELL2, CELL3 send data
    {
      // Wait next tranfer from master
      
      // If transfer is initiated
      if (SSI_CE_PA3 == SSI0_CE_ON)
      {
        SSI_send_byte(IR_CELL_MSB[count++]);
      }
      // Repeat until all IR sensor done
    }
  }
}


// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1
// ports PA0-1 are used for boot loader, not accessable ports
#include <stdint.h>
#include "UART.h"
#include "tm4c123gh6pm.h"

// standard ASCII symbols
#define CR   0x0D
#define LF   0x0A
#define BS   0x08
#define ESC  0x1B
#define SP   0x20
#define DEL  0x7F

// System clock is 80Mhz
// BRD = BRDI + BRDF = UARTSysClk / (ClkDiv * Baud Rate)
// IBRD = int(UARTSysClk / (ClkDiv * Baud Rate))
// FBRD = int(BRD * 64 + 0.5)
#define baud_9600_IBRD 520  // int( 80M / (16 * 9600)) = int(520.8333) = 520
#define baud_9600_FBRD 53   // int(0.8333 * 64 + 0.5) = int(53.8333) = 53
#define baud_128000_IBRD 39  // int( 80M / (16 * 128000)) = int(39.0625) = 39
#define baud_128000_FBRD 4 // int(0.0625 * 64 + 0.5) = int(4.5) = 4
#define baud_230400_IBRD 21  // int( 80M / (16 * 230400)) = int(21.7013) = 21
#define baud_230400_FRBD 45 // int(0.7013 * 64 + 0.5) = int(45.3888) = 45
#define baud_921600_IBRD 5  // int( 80M / (16 * 921600)) = int(5.4253) = 5
#define baud_921600_FRBD 27 // int(0.425 * 64 + 0.5) = int(27.722) = 27
#define baud_1M_IBRD 5  // int( 80M / (16 * 1M)) = int(5) = 5
#define baud_1M_FRBD 0 // int(0 * 64 + 0.5) = int(0.5) = 0

// PB0: UART1 Rx <--> Lidar Tx
// PB1: UART1 Tx <--> Lidar Rx
// PB3 -> DEV_EN(lidar)
void PortB_Init(void)
{
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;  // activate port B
  while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R1) == 0){};  //wait status of initialized clock

  // Alt function
  // Enable alt funct on PB1-0
  GPIO_PORTB_AFSEL_R |= 0x03;

  // enable digital I/O on PB1-0
  GPIO_PORTB_DEN_R |= 0x03; 
  
  // configure PB1-0 as UART
  GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB0_U1RX;
  GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB1_U1TX;

  // disable analog functionality on PB1-0
  GPIO_PORTB_AMSEL_R &= ~0x03;
  
  // disable pulldown resistor on PB1-0
	GPIO_PORTB_PDR_R &= ~0x03;
}


void UART0_Init(void){
  SYSCTL_RCGCUART_R |= 0x01;            // activate UART0
  SYSCTL_RCGCGPIO_R |= 0x01;            // activate port A
  while((SYSCTL_PRGPIO_R&0x01) == 0){}; //wait status of initialized clock
  UART0_CTL_R &= ~UART_CTL_UARTEN;      // disable UART
		
  //UART0_IBRD_R = baud_9600_IBRD;
  //UART0_FBRD_R = baud_9600_FBRD;
  //UART0_IBRD_R = baud_128000_IBRD;
  //UART0_FBRD_R = baud_128000_FBRD;
  //UART0_IBRD_R = baud_230400_IBRD;
  //UART0_FBRD_R = baud_230400_FRBD;
  UART0_IBRD_R = baud_921600_IBRD;
  UART0_FBRD_R = baud_921600_FRBD;
  //UART0_IBRD_R = baud_1M_IBRD;
  //UART0_FBRD_R = baud_1M_FRBD;
  
	// 8 bit word length (no parity bits, one stop bit, FIFOs)
  UART0_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
  UART0_CTL_R |= 0x301;                 // enable UART RXE AND TXE
  GPIO_PORTA_AFSEL_R |= 0x03;           // enable alt funct on PA1-0
  GPIO_PORTA_DEN_R |= 0x03;             // enable digital I/O on PA1-0
                                        // configure PA1-0 as UART
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00)+0x00000011;
  GPIO_PORTA_AMSEL_R &= ~0x03;          // disable analog functionality on PA
}

unsigned char UART0_InChar(void)
{
  // Receive FIFO

  // Wait if Receive FIFO is empty
  while((UART0_FR_R&UART_FR_RXFE) != 0);
  return((unsigned char)(UART0_DR_R&0xFF));
}

void UART0_OutChar(unsigned char data)
{
  // Transmit FIFO
  
  while((UART0_FR_R&UART_FR_TXFF) != 0);
  UART0_DR_R = data;
}

void UART0_HardFault(void)
{
  
}

// PB0: UART1 Rx <--> Lidar Tx
// PB1: UART1 Tx <--> Lidar Rx
void UART1_Init(void)
{
  SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1;  // activate UART1
  
  PortB_Init();

  UART1_CTL_R &= ~UART_CTL_UARTEN; // disable UART

  // In 128000 baud, 8bits, no parity
  UART1_IBRD_R = baud_128000_IBRD; 
  UART1_FBRD_R = baud_128000_FBRD;
  UART1_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);

  UART1_CTL_R |= UART_CTL_RXE;     // Enable UART RXE
  UART1_CTL_R |= UART_CTL_TXE;     // Enable UART TXE
    
  // UART1 Interrupt configuration
  // UART1_IM_R |= UART_IM_RXIM;
  // Set (7/8)bits will trigger
  
	//UART1_IFLS_R = ((~(0x3F)) | UART_IFLS_RX7_8);
  UART1_CTL_R |= UART_CTL_UARTEN;  // Enable UART
  //NVIC_PRI1_R |= ~(0xE00000);
  //NVIC_EN0_R |= 0x40; // IRQn = 6
}

unsigned char UART1_InChar(void)
{
  // Receive FIFO

  // Wait if Receive FIFO is empty
  while((UART1_FR_R&UART_FR_RXFE) != 0);
  return((unsigned char)(UART1_DR_R&0xFF));
}

void UART1_OutChar(unsigned char data)
{
  // Transmit FIFO

  while((UART1_FR_R&UART_FR_TXFF) != 0);
  UART1_DR_R = data;
}

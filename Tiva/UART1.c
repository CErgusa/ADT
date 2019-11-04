// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1
// ports PA0-1 are used for boot loader, not accessable ports
#include <stdint.h>
#include "UART1.h"
#include "tm4c123gh6pm.h"


#define UART_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART_CTL_UARTEN         0x00000001  // UART Enable

// also, 7 and 52?
int baud_128000_IBRD = 7;  // IBRD = int(16,000,000 / (16 * 115,200)) = int(43.4027)
int baud_128000_FBRD = 52; // FBRD = int(0.4027 * 64 + 0.5) = 27

int baud_9600_IBRD = 104;
int baud_9600_FBRD = 11;

int baud_230400_IBRD = 4;
int baud_230400_FRBD = 22;

// PB0: UART1 Rx
// PB1: UART1 Tx
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


void UART_Init(void){
  SYSCTL_RCGCUART_R |= 0x01;            // activate UART0
  SYSCTL_RCGCGPIO_R |= 0x01;            // activate port A
  while((SYSCTL_PRGPIO_R&0x01) == 0){}; //wait status of initialized clock
  UART0_CTL_R &= ~UART_CTL_UARTEN;      // disable UART
		
  UART0_IBRD_R = baud_230400_IBRD;
  UART0_FBRD_R = baud_230400_FRBD;
  
	// 8 bit word length (no parity bits, one stop bit, FIFOs)
  UART0_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
  UART0_CTL_R |= 0x301;                 // enable UART RXE AND TXE
  GPIO_PORTA_AFSEL_R |= 0x03;           // enable alt funct on PA1-0
  GPIO_PORTA_DEN_R |= 0x03;             // enable digital I/O on PA1-0
                                        // configure PA1-0 as UART
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00)+0x00000011;
  GPIO_PORTA_AMSEL_R &= ~0x03;          // disable analog functionality on PA
}

char UART_InChar(void){
  while((UART0_FR_R&UART_FR_RXFE) != 0);
  return((char)(UART0_DR_R&0xFF));
}

void UART_OutChar(char data){
  while((UART0_FR_R&UART_FR_TXFF) != 0);
  UART0_DR_R = data;
}


void UART1_Init(void)
{
  SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1;  // activate UART1
  
  PortB_Init();

  UART1_CTL_R &= ~UART_CTL_UARTEN; // disable UART

  // In 128000 baud, 8bits, no parity
  UART1_IBRD_R = baud_128000_IBRD;   // IBRD = int(16,000,000 / (16 * 9600)) = int(104.16666)   896p in datasheet
  UART1_FBRD_R = baud_128000_FBRD;  // FBRD = int(0.68056 * 64 + 0.5) = int(11.66624)
  UART1_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);

  UART1_CTL_R |= UART_CTL_RXE;     // Enable UART RXE
  UART1_CTL_R |= UART_CTL_TXE;     // Enable UART TXE
  UART1_CTL_R |= UART_CTL_UARTEN;  // Enable UART
  

}

char UART1_InChar(void){
  while((UART1_FR_R&UART_FR_RXFE) != 0);
  return((char)(UART1_DR_R&0xFF));
}

void UART1_OutChar(char data){
  while((UART1_FR_R&UART_FR_TXFF) != 0);
  UART1_DR_R = data;
}

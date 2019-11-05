// UART.c
// Runs on LM3S811, LM3S1968, LM3S8962, LM4F120, TM4C123
// Simple device driver for the UART.
// Daniel Valvano
// May 30, 2014
// Modified by EE345L students Charlie Gough && Matt Hawk
// Modified by EE345M students Agustinus Darmawan && Mingjie Qiu

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Program 4.12, Section 4.9.4, Figures 4.26 and 4.40

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1
#include <stdint.h>
#include "UART.h"
#include "tm4c123gh6pm.h"


#define UART_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART_CTL_UARTEN         0x00000001  // UART Enable


//------------UART_Init------------
// Initialize the UART for 115,200 baud rate (assuming 16 MHz UART clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void UART0_Init(void)
{
  SYSCTL_RCGCUART_R |= 0x01;            // activate UART0
  SYSCTL_RCGCGPIO_R |= 0x01;            // activate port A
  while((SYSCTL_PRGPIO_R&0x01) == 0){};  //wait status of initialized clock
  UART0_CTL_R &= ~UART_CTL_UARTEN;      // disable UART
    
  // In 9600 baud, 8bits, no parity
  UART0_IBRD_R = 104; // IBRD = int(16,000,000 / (16 * 9600)) = int(104.16666)   896p in datasheet
  UART0_FBRD_R = 11;  // FBRD = int(0.68056 * 64 + 0.5) = int(11.66624)
  UART0_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);

		
  UART0_CTL_R |= 0x301;                 // enable UART RXE AND TXE
  GPIO_PORTA_AFSEL_R |= 0x03;           // enable alt funct on PA1-0
  GPIO_PORTA_DEN_R |= 0x03;             // enable digital I/O on PA1-0
                                        // configure PA1-0 as UART
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00)+0x00000011;
  GPIO_PORTA_AMSEL_R &= ~0x03;          // disable analog functionality on PA
}


//------------UART_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
char UART0_InChar(void){
  while((UART0_FR_R&UART_FR_RXFE) != 0);
  return((char)(UART0_DR_R&0xFF));
}

//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART0_OutChar(char data){
  while((UART0_FR_R&UART_FR_TXFF) != 0);
  UART0_DR_R = data;
}


void UART0_OutString(char *data)
{
  int i = 0;
  while (data[i] != ';')
  {
    UART0_OutChar(data[i]);
    ++i;
  }
}


// PB0(UART1 Rx) <--> LCD Tx
// PB1(UART1 Tx) <--> LCD Rx
void UART1_Init(void)
{
  SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1;  // activate UART1
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;  // activate port B
  while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R1) == 0){};  //wait status of initialized clock
  
  UART1_CTL_R &= ~UART_CTL_UARTEN; // disable UART
    
  // In 9600 baud, 8bits, no parity
  UART1_IBRD_R = 104; // IBRD = int(16,000,000 / (16 * 9600)) = int(104.16666)   896p in datasheet
  UART1_FBRD_R = 11;  // FBRD = int(0.68056 * 64 + 0.5) = int(11.66624)
  UART1_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
 
  UART1_CTL_R |= UART_CTL_RXE;     // Enable UART RXE
  UART1_CTL_R |= UART_CTL_TXE;     // Enable UART TXE
  UART1_CTL_R |= UART_CTL_UARTEN;  // Enable UART
    
  GPIO_PORTB_AFSEL_R |= 0x03;           // enable alt funct on PB1-0
  GPIO_PORTB_DEN_R |= 0x03;             // enable digital I/O on PB1-0
  GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB0_U1RX; // configure PB1-0 as UART
  GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB1_U1TX;
   
  GPIO_PORTB_AMSEL_R &= ~0x03;          // disable analog functionality on PB
}


char UART1_InChar(void){
  while((UART1_FR_R&UART_FR_RXFE) != 0);
  return((char)(UART1_DR_R&0xFF));
}


void UART1_OutChar(char data){
  while((UART1_FR_R&UART_FR_TXFF) != 0);
  UART1_DR_R = data;
}


void UART1_OutString(char *data)
{
  int i = 0;
  while (data[i] != ';')
  {
    UART1_OutChar(data[i]);
    ++i;
  }
}


// PE4(UART5 Rx) <--> Xbee DOUT
// PE5(UART5 Tx) <--> Xbee DIN
void UART5_Init(void)
{
  SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R5;  // activate UART4
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;  // activate port E
  while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R4) == 0){};  //wait status of initialized clock
  
  UART5_CTL_R &= ~UART_CTL_UARTEN; // disable UART
    
  // In 9600 baud, 8bits, no parity
  UART5_IBRD_R = 104; // IBRD = int(16,000,000 / (16 * 9600)) = int(104.16666)   896p in datasheet
  UART5_FBRD_R = 11;  // FBRD = int(0.68056 * 64 + 0.5) = int(11.66624)
  UART5_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
 
  UART5_CTL_R |= UART_CTL_RXE;     // Enable UART RXE
  UART5_CTL_R |= UART_CTL_TXE;     // Enable UART TXE
  UART5_CTL_R |= UART_CTL_UARTEN;  // Enable UART
    
  GPIO_PORTE_AFSEL_R |= 0x30;           // enable alt funct on PD7-6
  GPIO_PORTE_DEN_R |= 0x30;             // enable digital I/O on PD7-6
  GPIO_PORTE_PCTL_R |= GPIO_PCTL_PE4_U5RX; // configure PD7-6 as UART
  GPIO_PORTE_PCTL_R |= GPIO_PCTL_PE5_U5TX;
   
  GPIO_PORTE_AMSEL_R &= ~0x30;          // disable analog functionality on PD7-6
}


char UART5_InChar(void){
  while((UART5_FR_R&UART_FR_RXFE) != 0);
  return((char)(UART5_DR_R&0xFF));
}


void UART5_OutChar(char data){
  while((UART5_FR_R&UART_FR_TXFF) != 0);
  UART5_DR_R = data;
}


void UART5_OutString(char *data)
{
  int i = 0;
  while (data[i] != ';')
  {
    UART5_OutChar(data[i]);
    ++i;
  }
}

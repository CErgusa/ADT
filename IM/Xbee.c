#include "Xbee.h"
#include "UART.h"

void XBee_Wait(void)
{
  int i;
  for (i = 0; i < (1600000*5); ++i);
}

void Xbee_SetDefault(void)
{
  UART1_OutString("+++;");
  UART5_OutString("+++;");
  XBee_Wait();
  
  char response = UART5_InChar();
  
  UART1_OutString("ATRE;");
  UART5_OutString("ATRE;");
  UART1_OutChar(0xD);  
  UART5_OutChar(0xD);
  XBee_Wait();  
  
  UART1_OutString("ATCN;");
  UART5_OutString("ATCN;");
  UART1_OutChar(0xD);    
  UART5_OutChar(0xD);
  XBee_Wait();  
}


void Xbee_ChangePANid(char *_id)
{
  UART1_OutString("+++;");
  UART5_OutString("+++;");
  XBee_Wait();  
  
  UART1_OutString("ATID ;");  
  UART5_OutString("ATID ;");
  
  UART1_OutString(_id);  
  UART5_OutString(_id);
  
  UART1_OutChar(0xD);  
  UART5_OutChar(0xD);
  XBee_Wait();  

  UART1_OutString("ATCN;");
  UART5_OutString("ATCN;");
  
  UART1_OutChar(0xD); 
  UART5_OutChar(0xD);
  XBee_Wait();  
}


void Xbee_ChangeBaudRate(char *_baudRate)
{
  UART1_OutString("+++;");
  UART5_OutString("+++;");
  XBee_Wait();  

  UART1_OutString("ATBD ;");  
  UART5_OutString("ATBD ;");
  
  UART1_OutString(_baudRate);  
  UART5_OutString(_baudRate);
  UART1_OutChar(0xD);  
  UART5_OutChar(0xD);
  XBee_Wait();  

  UART1_OutString("ATCN;");  
  UART5_OutString("ATCN;");

  UART1_OutChar(0xD);  
  UART5_OutChar(0xD);
  XBee_Wait();  
}


void Xbee_ADlink(void)
{
  UART1_OutString("+++;");
  UART5_OutString("+++;");
  XBee_Wait();
  
  UART1_OutString("ATDL 1234;");  
  UART5_OutString("ATDL 1234;");
  UART1_OutChar(0xD);  
  UART5_OutChar(0xD);
  XBee_Wait();
  
  UART1_OutString("ATMY 5678;");  
  UART5_OutString("ATMY 5678;");
  UART1_OutChar(0xD);  
  UART5_OutChar(0xD);
  XBee_Wait();  
  
  UART1_OutString("ATD1 2;");  
  UART5_OutString("ATD1 2;");
  UART1_OutChar(0xD);  
  UART5_OutChar(0xD);
  XBee_Wait();
  
  UART1_OutString("ATIR 64;");  
  UART5_OutString("ATIR 64;");
  UART1_OutChar(0xD);  
  UART5_OutChar(0xD);
  XBee_Wait();
  
  UART1_OutString("ATIT 1;");  
  UART5_OutString("ATIT 1;");
  UART1_OutChar(0xD);  
  UART5_OutChar(0xD);
  XBee_Wait();
  
  UART1_OutString("ATCN;");  
  UART5_OutString("ATCN;");
  UART1_OutChar(0xD);  
  UART5_OutChar(0xD);
  XBee_Wait();
}

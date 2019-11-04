#include <stdbool.h>

void LCD_UART_Init(void);

void LCD_UART_Clear(void);

void LCD_UART_NewLine(void);

void LCD_UART_OutChar(char _char);

void LCD_UART_OutString(char *string, bool _NeedNewLine);

void LCD_UART_ModeSetup(void);

void LCD_UART_NewLine(void);

void LCD_UART_BackSpace(void);

void LCD_UART_Previewing(char _temp);

void LCD_UART_OutDataInFormat(char * _ConfirmedData);

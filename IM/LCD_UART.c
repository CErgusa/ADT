#include "LCD_UART.h"
#include "UART.h"
#include "timer.h"

int LineIndex;
int CursorPos;

#define DECREMENT 0x00
#define INCREMENT 0x01

#define LINE_0 0x00
#define LINE_1 0x40
#define LINE_2 0x14
#define LINE_3 0x54


// PB0: Rx
// PB1: Tx




void IncrementLineIndex(int Direction)
{
  if (Direction == INCREMENT)
  {
    switch (LineIndex) // Increment the line
    {
      case LINE_0: // Currently line 0
      {
        LineIndex = LINE_1;
        break;
      }
      case LINE_1: // Currently line 1
      {
        LineIndex = LINE_2;
        break;
      }
      case LINE_2: // Currently line 2
      {
        LineIndex = LINE_3;
        break;
      }
      case LINE_3: // Currently line 3
      {
        LineIndex = LINE_0;
        LCD_UART_Clear();
        break;
      }
    }
  }
  else
  {
    switch (LineIndex) // Decrement the line
    {
      case LINE_0: // Currently line 0
      {
        LineIndex = LINE_3; // Go to 4
        break;
      }
      case LINE_1: // Currently line 1
      {
        LineIndex = LINE_0; // Go to 0
        break;
      }
      case LINE_2: // Currently line 2
      {
        LineIndex = LINE_1; // Go to 1
        break;
      }
      case LINE_3: // Currently line 3
      {
        LineIndex = LINE_2;
        //LCD_UART_Clear();
        break;
      }
    }
  }
}


void IncrementCursor(int Direction)
{
  if (Direction == INCREMENT)
  {
    ++CursorPos;
    if (CursorPos == 0x14)
    {
      IncrementLineIndex(INCREMENT);
      CursorPos = 0;
    }
  }
  else
  {
    --CursorPos;
    if (CursorPos < 0x0)
    {
      IncrementLineIndex(DECREMENT);
      CursorPos = 0x13;
    }
  }
}



void LCD_UART_SetCursorPos(char _pos)
{
  UART1_OutChar(0xFE);
  
  UART1_OutChar(0x80+_pos);
}

void LCD_UART_Init(void)
{
  LineIndex = 0;
  CursorPos = 0;
  // Reset to 9600BR
  //UART1_OutChar(0x12);
  
  // Give time to LCD to be initialized
  Wait_ms(500);

  LCD_UART_Clear();  
  
  LCD_UART_ModeSetup();
}

void LCD_UART_ModeSetup(void)
{
  // Backlight: Blue 50% fully on
  //UART1_OutChar(0x7C);
  //Wait_us(50);
  //UART1_OutChar(0x80); // RED 0%
  //UART1_OutChar(0xD2); // BLUE about 50%
  //Wait_ms(1);
  
  // Blinking cursor on
  UART1_OutChar(0xFE);
  Wait_us(50);
  UART1_OutChar(0x0D);
  Wait_us(50);
}


void LCD_UART_Clear(void)
{
  // Need to clear the screen
  UART1_OutChar(0xFE); // 20x4 screen
  //UART1_OutChar('|');  // 16x2 screen
  //Wait_us(50);
  
  UART1_OutChar(0x01); // 20x4 screen
  //UART1_OutChar('-');    // 16x2 screen
  
  LineIndex = 0;
  CursorPos = 0;
  
  Wait_us(50);
}


void LCD_UART_NewLine(void)
{
  //UART0_OutChar(' ');
  IncrementLineIndex(INCREMENT);

  LCD_UART_SetCursorPos(LineIndex);
  
  
  //UART1_OutChar(0xFE);
  //UART1_OutChar(LineConstant + LineIndex);
   
  CursorPos = 0;
  
  Wait_us(50);
}




void LCD_UART_OutChar(char _char)
{
  if (_char != ';' && _char != 0x10)
  {
    //UART0_OutChar(_char);
    UART1_OutChar(_char);
    IncrementCursor(INCREMENT);
  }
}


void LCD_UART_OutString(char *string, bool _NeedNewLine)
{
  int i = 0;
  int LineIndexBeforeInserting = LineIndex;
  while (1)
  {
    if (string[i] == ';')
    {
      break;
    }
    LCD_UART_OutChar(string[i]);
    ++i;
  }
  
  if (_NeedNewLine)
  {
    // If the line is still the same after inserting the string
    if (LineIndexBeforeInserting == LineIndex)
    {
      LCD_UART_NewLine();
    }
  }
}


void LCD_UART_BackSpace(void)
{  
  // Move one left
  IncrementCursor(DECREMENT);
  LCD_UART_SetCursorPos(LineIndex + CursorPos);
  
  // Insert empty char
  LCD_UART_OutChar(' ');
  
  // UART1_OutChar(' '); 
  
  // Move one left again
  IncrementCursor(DECREMENT);
  LCD_UART_SetCursorPos(LineIndex + CursorPos);
 
}

void LCD_UART_Previewing(char _temp)
{
  LCD_UART_OutChar(_temp);
  IncrementCursor(DECREMENT);
  
  LCD_UART_SetCursorPos(LineIndex+CursorPos);
}


void LCD_UART_OutDataInFormat(char * _ConfirmedData)
{
  int i = 3;
  LCD_UART_OutString("<N>:;" , false);
  
  while (_ConfirmedData[i] != '<')
  {
    LCD_UART_OutChar(_ConfirmedData[i]);
    ++i;
  }
  
  LCD_UART_NewLine();
  LCD_UART_OutString("<R>:;" , false);
  i += 3;  
  while (_ConfirmedData[i] != '<')
  {
    LCD_UART_OutChar(_ConfirmedData[i]);
    ++i;
  }
  
  LCD_UART_NewLine();
  LCD_UART_OutString("<C>:;" , false);
  i += 3;
  while (_ConfirmedData[i] != ';')
  {
    LCD_UART_OutChar(_ConfirmedData[i]);
    ++i;
  }
}

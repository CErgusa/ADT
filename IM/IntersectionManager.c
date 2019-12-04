#include "IntersectionManager.h"
#include "UART.h"
#include "SPI.h"
#include "Xbee.h"
#include "LCD_SPI.h"
#include "LCD_UART.h"
#include "adc.h"
#include "timer.h"


// PA0 <--> PC
// PA1 <--> PC
// PB0(UART1 Rx) <--> LCD TXO
// PB1(UART1 Tx) <--> LCD RXI
// PD0(AIN7) -> Cell3 -> ADC0
// PD1(AIN6) -> Cell2 -> ADC0
// PD2(AIN5) -> Cell1 -> ADC0
// PD3(AIN4) -> Cell4 -> ADC1
// PE4(UART5 Rx) <--> Xbee DOUT
// PE5(UART5 Tx) <--> Xbee DIN

void IM_shutdown(void)
{
 
}

bool IM_cell_safe(uint32_t *cell)
{
  int i = 0;
  for (i = 0; i < 4; ++i)
  {
    // If any of 4 cells is dead
    if (cell[i] == 0)
    {
      return false;
    }
  }
  return true;
}

void IM_read_cell(uint32_t *cell)
{
  cell[0] = ADC_Get(0, CELL1_CHANNEL);
  cell[1] = ADC_Get(0, CELL2_CHANNEL);
  cell[2] = ADC_Get(0, CELL3_CHANNEL);
  cell[3] = ADC_Get(1, CELL4_CHANNEL);
}


void IM_print_cell(uint32_t *cell_raw)
{
  char cell_msb[4];
  cell_msb[0] = (cell_raw[0] >> 4) & 0xFF;
  cell_msb[1] = (cell_raw[1] >> 4) & 0xFF;
  cell_msb[2] = (cell_raw[2] >> 4) & 0xFF;
  cell_msb[3] = (cell_raw[3] >> 4) & 0xFF;
  
  LCD_UART_OutString("CELL:;", false);
  int i = 0;
  int j = 0;
  char cell_hex[3];
  for (i = 0; i < 4; ++i)
  {
    for (j = 0; j < 3; ++j)
    {
      char four_bits = 0;
      // first 4bit
      if (j == 2)
      {
        cell_hex[j] = ';';
      }
      else
      {
        if (j == 0)
        {
          four_bits = (cell_msb[i] & 0xF0) >> 4;
        }
        else // j == 1
        {
          four_bits = cell_msb[i] & 0x0F;
        }
        
        if (four_bits <= 0x9)
        {
          cell_hex[j] = four_bits + 48;
        }
        else
        {
          cell_hex[j] = four_bits + 55;
        }
      }
    }
    LCD_UART_OutString(cell_hex, false);
    LCD_UART_OutChar(' ');
  }
  LCD_UART_NewLine();
}

void IM_send_control_tank(char dir)
{
  UART0_OutChar(dir);
  UART5_OutChar(dir);
  LCD_UART_OutChar(dir);
}

void IntersectionManager_Init(void)
{
  // Initialize UART: boud rate, bits, stop bit, parity, flow control
	UART0_Init(); // Teraterm
  UART1_Init(); // LCD
  UART5_Init(); // Xbee
  ADC0_Init();  // Cell 1,2,3
  ADC1_Init();  // Cell 4

  LCD_UART_Init();
  
  //UART0_OutString("SYSTEM START;");
  LCD_UART_OutString("SYSTEM START: IM;", true);
  Wait_ms(3000);
}

void IntersectionManager_loop(void)
{
  LCD_UART_Clear();
  
  uint32_t cell[4];
  IM_read_cell(cell);
  
  // cell voltage check
  if (!IM_cell_safe(cell))
  {
    IM_shutdown();
  }
  
  // Always put the cell voltage on the top
  IM_print_cell(cell);
  
  char dir = UART0_InChar();
  
  IM_send_control_tank(dir);
  
  Wait_ms(1000);
}

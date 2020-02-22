#include "system.h"
#include "SysTick (2).h"
#include "adc.h"
#include "SSI.h"
#include "UART1.h"
#include "Lidar.h"
#include "tm4c123gh6pm.h"
#include "GDL.h"
#include "FPU.h"


void clock_check(void)
{
  int i = 1;
  while(1)
  {
    SysTick_Wait1us(1);
    GDL_Send(i);
    i = ~i;
  }
}

void system_IR_cell_add_packet(char *buffer)
{
  // Send IR, Cell data
  // Only MSB 8bits
  buffer[MAX_PACKET_SIZE] = 11;//ADC_Get(1, IR1_CHANNEL) >> 4;
  buffer[MAX_PACKET_SIZE+1] = 22;//ADC_Get(1, IR2_CHANNEL) >> 4;
  buffer[MAX_PACKET_SIZE+2] = 33;//ADC_Get(1, IR3_CHANNEL) >> 4;
  buffer[MAX_PACKET_SIZE+3] = 44;//ADC_Get(0, CELL1_CHANNEL) >> 4;
  buffer[MAX_PACKET_SIZE+4] = 55;//ADC_Get(0, CELL2_CHANNEL) >> 4;
  buffer[MAX_PACKET_SIZE+5] = 66;//ADC_Get(0, CELL3_CHANNEL) >> 4;
}


void system_init(void)
{
	SysTick_Init(); // System timer
  clock_check();
  FPU_Init();     // Enable Floating Point Unit
  SSI_Init();     // SPI communication
  UART_Init();    // TeraTerm UART communication
	ADC0_Init();    // Batter cell voltage
  ADC1_Init();    // IR sensors
	UART1_Init();   // Lidar UART Communication
	GDL_Init();     // General Data Logic
}

void system_send(char *buffer)
{
  // Trigger GPIO interrupt of Rpi
  GDL_Send(0x01);
  int i;
  for (i = 0; i < MAX_PACKET_SIZE+6; ++i)
  {
    SSI_send_byte(buffer[i]);
  }
  // Reset GPIO interrupt
  GDL_Send(0x00);
}


// Jun change this as needed, this is our new main function
// I just wanted to move things towards an engine rather than
// always being in main!
int system_engine(void)
{
  // Before the first run, Stop Lidar first
  stop_lidar();

  // get buffer for sample array

  lidar_scan_response();

  while (1)
  {
    char buffer[MAX_PACKET_SIZE] = { 0 };
    // get rest of the packet
    int success = lidar_get_packet(buffer);

    if (success)
    {
      continue;
      //system_IR_cell_add_packet(buffer);
      //system_send(buffer);
    }
    else // Some problem occur, not aligned correctly?
    {
      stop_lidar();
      // get buffer for sample array
      lidar_scan_response();
    }
  }


  return ERROR;
}

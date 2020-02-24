#include "system.h"
#include "PLL.h"
#include "systick.h"
#include "FPU.h"
#include "SSI.h"
#include "UART.h"
#include "ADC.h"
#include "GDL.h"
#include "lidar.h"

#define ERROR 0
#define NO_ERROR 1
#define MAX_PACKET_SIZE 167 // Size[1] + Angle[40*2] + Dist40*2] + IR[3] + Voltage[3]


void clock_check_loop(void)
{
  int i = 1;
  char j = '>';
  float test1 = 1.2;
  int test2 = (int)(test1 * 100.f);
  while (1)
  {
    test1 *= 1.2f * i;
    
    if (test1 > 2.f)
    {
      UART0_OutChar('y');
    }
    
    if (test1 > 4.f)
    {
      UART0_OutChar('5');
    }
    ++i;
  }
}



void system_init(void)
{
  PLL_init();
	systick_init(); // System timer
  FPU_init();
  SSI_Init();
  UART0_Init();    // TeraTerm UART communication
	UART1_Init();   // Lidar UART Communication
	ADC0_Init();    // Batter cell voltage
  ADC1_Init();    // IR sensors
	GDL_init();     // General Data Logic
}

void system_send(unsigned char *buffer)
{
  // Trigger GPIO interrupt of Rpi
  GDL_send(0x01);
  int i;
  int size = buffer[0] + 6;
  for (i = 0; i < size; ++i)
  {
    SSI_send_byte(buffer[i]);
  }
  // Reset GPIO interrupt
  GDL_send(0x00);
}

void system_IR_cell_add_packet(unsigned char *buffer)
{
  // Send IR, Cell data
  // Only MSB 8bits
  unsigned char actual_size = buffer[0];
  buffer[actual_size] = 11;//ADC_Get(1, IR1_CHANNEL) >> 4;
  buffer[actual_size + 1] = 22;//ADC_Get(1, IR2_CHANNEL) >> 4;
  buffer[actual_size + 2] = 33;//ADC_Get(1, IR3_CHANNEL) >> 4;
  buffer[actual_size + 3] = 44;//ADC_Get(0, CELL1_CHANNEL) >> 4;
  buffer[actual_size + 4] = 55;//ADC_Get(0, CELL2_CHANNEL) >> 4;
  buffer[actual_size + 5] = 66;//ADC_Get(0, CELL3_CHANNEL) >> 4;
}

int system_engine(void)
{
  clock_check_loop();
  lidar_stop_command();
  // get buffer for sample array
  lidar_scan_response();

  while (1)
  {
    unsigned char buffer[MAX_PACKET_SIZE] = { 0 };
    // get rest of the packet
    int success = lidar_get_packet(buffer);

    if (success)
    {
      system_IR_cell_add_packet(buffer);
      system_send(buffer);
      continue;
    }
    else // Some problem occur, not aligned correctly?
    {
      lidar_stop_command();
      // get buffer for sample array
      lidar_scan_response();
    }
  }

  return ERROR;
}

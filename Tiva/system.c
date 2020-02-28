#include "system.h"
#include "PLL.h"
#include "systick.h"
#include "FPU.h"
#include "SSI.h"
#include "UART.h"
#include "ADC.h"
#include "GDL.h"
#include "lidar.h"
#include "utils.h"

#include <stdio.h> // sizeof

#define MAX_PACKET_SIZE 167 // Size[1] + Angle[40*2] + Dist[40*2] + IR[3] + Voltage[3]


void clock_check_loop(void)
{
  return;
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
  unsigned char i;
  for (i = 0; i <= buffer[0]; ++i)
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
  
  unsigned char IR_Cell_start = 1 + buffer[0]; // ~ [161]
  buffer[IR_Cell_start] = 0xAA;//ADC_Get(1, IR1_CHANNEL) >> 4;
  buffer[IR_Cell_start + 1] = 0xBB;//ADC_Get(1, IR2_CHANNEL) >> 4;
  buffer[IR_Cell_start + 2] = 0xCC;//ADC_Get(1, IR3_CHANNEL) >> 4;
  buffer[IR_Cell_start + 3] = 0xDD;//ADC_Get(0, CELL1_CHANNEL) >> 4;
  buffer[IR_Cell_start + 4] = 0xEE;//ADC_Get(0, CELL2_CHANNEL) >> 4;
  buffer[IR_Cell_start + 5] = 0xFF;//ADC_Get(0, CELL3_CHANNEL) >> 4;
  
  // Update the size
  buffer[0] += 6;
}

int system_engine(void)
{

  //float_debugging(0.0f);
  //clock_check_loop();

  //unsigned char size_uchar= sizeof(unsigned char);
  //unsigned char size_int = sizeof(int);  
  //unsigned char size_float = sizeof(float);
  //UART0_OutChar(value_to_char(size_uchar));
  //UART0_OutChar(value_to_char(size_int));
  //UART0_OutChar(value_to_char(size_float));
  
  
  lidar_stop_command();

  lidar_scan_response();

  unsigned char buffer[MAX_PACKET_SIZE] = { 0 };  
  
  while (1)
  {
    UART0_OutChar('a');
    // get rest of the packet
    int packet_type = lidar_get_packet(buffer);

    if (packet_type == 0x00) // Point cloud
    {
      system_IR_cell_add_packet(buffer);
      system_send(buffer);
    }
    
    int i;
    for (i = 0; i < MAX_PACKET_SIZE; ++i)
    {
      buffer[i] = 0;
    }
    UART0_OutChar('h');
  }

  return 0;
}

#include "system.h"
#include "PLL.h"
#include "systick.h"
#include "FPU.h"
#include "SSI.h"
#include "UART.h"
#include "ADC.h"
#include "GDL.h"
#include "lidar.h"


// | Size |  x  |  y  |  theta  |  r  |  IR  |  Cell  |
//   1   +   80 +  80 +  80     +  80 +  3   + 3 = 327
#define MAX_PACKET_SIZE 327
#define AVERAGED_SIZE 


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

unsigned char system_RPi_ready(void)
{
  return GDL_read(0);
}

void system_send(unsigned char *buffer)
{
//  unsigned char buffer[MAX_PACKET_SIZE] = { 0 };
//  buffer[0] = 0x28;
//  int k;
//  for (k = 1; k < MAX_PACKET_SIZE; ++k)
//  {
//    buffer[k] = k;
//  }

  // Set the ready flag in RPi
  //while (1)
  //{
    if (GDL_read(0) == 1)
    {
      unsigned char response = SSI_read_byte(0xAC);
      if (response == 0xAC)
      {
        // size + (n * 2 * 4) + 6
        int buffer_size = 1+(8*buffer[0])+6;
        
        int i;
        for (i = 0; i < buffer_size; ++i)
        {
          SSI_send_byte(buffer[i]);
        }
      }
    }
  //}
}

void system_IR_cell_add_packet(unsigned char *buffer)
{
  // Send IR, Cell data
  // Only MSB 8bits
  unsigned char n = buffer[0];
  int IR_Cell_start = 1;
  if (n > 0)
  {
    IR_Cell_start = (8 * n) + 1;
  }

  buffer[IR_Cell_start] = ADC_Get(1, IR1_CHANNEL) >> 4;
  buffer[IR_Cell_start + 1] = ADC_Get(1, IR2_CHANNEL) >> 4;
  buffer[IR_Cell_start + 2] = ADC_Get(1, IR3_CHANNEL) >> 4;
  buffer[IR_Cell_start + 3] = 0xDD;//ADC_Get(0, CELL1_CHANNEL) >> 4;
  buffer[IR_Cell_start + 4] = 0xEE;//ADC_Get(0, CELL2_CHANNEL) >> 4;
  buffer[IR_Cell_start + 5] = 0xFF;//ADC_Get(0, CELL3_CHANNEL) >> 4;
}

void system_data_communication(unsigned char *buffer)
{
  //UART0_OutChar('a');
  // get rest of the packet
  int packet_type = lidar_get_packet(buffer);

  if (packet_type == 0x00) // Point cloud
  {
    if (buffer[0] != 0x00)
    {
      system_IR_cell_add_packet(buffer);
      system_send(buffer);
    }
  }
  
  // Must need this part to reserve the stack
  int i;
  for (i = 0; i < MAX_PACKET_SIZE; ++i)
  {
    buffer[i] = 0;
  }
  
  //UART0_OutChar('f');
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
    system_data_communication(buffer);
  }
  return 0;
}

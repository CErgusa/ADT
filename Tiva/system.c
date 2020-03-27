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
//#include "motor_control.h"

#include <stdio.h> // sizeof


// x = r*sin(theta)
// y = r*cos(theta)

// | Size |  x  |  y  |  theta  |  r  |  IR  |  Cell  |
//   1   +   80 +  80 +  80     +  80 +  3   + 3 = 327
#define MAX_PACKET_SIZE 327


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
  //motor_control_Init() // Motor Controls
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
  
  unsigned char n = buffer[0];
  int IR_Cell_start = 0;
  if (n > 0)
  {
    IR_Cell_start = (8 * n) + 9;
  }

  buffer[IR_Cell_start] = 0xAA;//ADC_Get(1, IR1_CHANNEL) >> 4;
  buffer[IR_Cell_start + 1] = 0xBB;//ADC_Get(1, IR2_CHANNEL) >> 4;
  buffer[IR_Cell_start + 2] = 0xCC;//ADC_Get(1, IR3_CHANNEL) >> 4;
  buffer[IR_Cell_start + 3] = 0xDD;//ADC_Get(0, CELL1_CHANNEL) >> 4;
  buffer[IR_Cell_start + 4] = 0xEE;//ADC_Get(0, CELL2_CHANNEL) >> 4;
  buffer[IR_Cell_start + 5] = 0xFF;//ADC_Get(0, CELL3_CHANNEL) >> 4;
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
  int i;

  //uint32_t front_IR;
  
  while (1)
  {
    UART0_OutChar('a');
    // get rest of the packet
    int packet_type = lidar_get_packet(buffer);

    if (packet_type == 0x00) // Point cloud
    {
      system_IR_cell_add_packet(buffer);
      //system_send(buffer);
    }
    UART0_OutChar('f');
    
    for (i = 0; i < MAX_PACKET_SIZE; ++i)
    {
      buffer[i] = 0;
    }

    //front_IR = ADC_Get(1, IR2_CHANNEL);
    //motor_testing(front_IR);
  }
  return 0;
}

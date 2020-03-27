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

// | PH | CT | LSN | FSA | LSA | CS | Sn |
//   2  + 1  +  1  +  2  +  2  +  2 + 80 = 90
#define MAX_LIDAR_RAW_PACKET_SIZE 90

// | Size | theta | r    |
//   1char  4char   2char
//   1    + 160   + 80
#define MAX_LIDAR_COMPUTED_PACKET_SIZE 241

#define MAX_NUM_PACKETS_PER_ZERO 15

// 15 * MAX_LIDAR_COMPUTED_PACKET_SIZE = 15 * 241 = 3615
#define MAX_LIDAR_ROTATION_DATA (MAX_NUM_PACKETS_PER_ZERO * MAX_LIDAR_COMPUTED_PACKET_SIZE)

// | MAX_LIDAR_ROTATION_DATA | MAX_LIDAR_RAW_PACKET_SIZE |
//          3615             +           90              = 3705
#define MAX_LIDAR_DATA_SIZE (MAX_LIDAR_ROTATION_DATA+MAX_LIDAR_RAW_PACKET_SIZE)

// | Size |  x  |  y  |  theta  |  r  |  IR  |  Cell  |
//   1   +   80 +  80 +  80     +  80 +  3   + 3 = 327
#define MAX_PACKET_SIZE 327


void clock_check_loop(void)
{
  return;
}

void system_flush_buffer(unsigned char *buffer, int size)
{
  // Must need this part to reserve the stack
  int i;
  for (i = 0; i < size; ++i)
  {
    buffer[i] = 0;
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

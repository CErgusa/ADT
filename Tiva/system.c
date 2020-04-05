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

void system_update_to_average(float *angle_windows, int *dist_windows, unsigned char *target_angle, unsigned char* target_dist)
{
  unsigned char i;
  int average_dist = 0;
  int valid_dist = 0;
  float average_angle = 0.f;
  float valid_angle = 0.0f;
  for (i = 0; i < 3; ++i)
  {
    if (angle_windows[i] > 0.0f)
    {
      average_angle += angle_windows[i];
      valid_angle += 1.0f;
    }
    
    if (dist_windows[i] > 0)
    {
      average_dist += dist_windows[i];
      ++valid_dist;
    }
  }
  average_angle = average_angle / valid_angle;
  average_dist = average_dist / valid_dist;
  
  store_float_in_4char(&average_angle, target_angle);
  store_int_in_2char(average_dist, target_dist);
}


void system_lidar_averaging(unsigned char num_packets, unsigned char **packets)
{
  unsigned char i;
  float angle_window[3] = { 0.f };
  int dist_window[3] = { 0 };
  unsigned char *angle_target_addr = 0;
  unsigned char *distance_target_addr = 0;

  for (i = 0; i < num_packets; ++i)
  {
    unsigned char j;
    unsigned char *packet_i_th = packets[i];
    unsigned char size_i_th = packet_i_th[0];

    for (j = 1; j <= size_i_th; ++j)
    {
      unsigned char angle_index = 4*(j-1)+1;
      unsigned char dist_index = 2*(j-1) + (4*size_i_th) + 1;
      store_4char_in_float(&packet_i_th[angle_index], &angle_window[2]);
      store_2char_in_int(&packet_i_th[dist_index], &dist_window[2]);
      
      if (angle_target_addr && distance_target_addr)
      {
        system_update_to_average(angle_window, dist_window, angle_target_addr, distance_target_addr);
      }
      
      angle_window[0] = angle_window[1];
      angle_window[1] = angle_window[2];
      
      dist_window[0] = dist_window[1];
      dist_window[1] = dist_window[2];
      
      angle_target_addr = &packet_i_th[angle_index];
      distance_target_addr = &packet_i_th[dist_index];
    }
  }
  angle_window[2] = 0.f;
  dist_window[2] = 0;
  system_update_to_average(angle_window, dist_window, angle_target_addr, distance_target_addr);
}

int system_count_total_buffer(unsigned char num_packets, unsigned char **packets)
{
  unsigned char i;
  int num_total_samples = 0;
  for (i = 0; i < num_packets; ++i)
  {
    num_total_samples += packets[i][0];
  }
  return (6 * num_total_samples) + num_packets;
}

void system_IR_cell_add_packet(unsigned char *buffer)
{
  buffer[0] = 0xFF - (ADC_Get(1, IR1_CHANNEL) >> 4);
  buffer[1] = 0xFF - (ADC_Get(1, IR2_CHANNEL) >> 4);
  buffer[2] = 0xFF - (ADC_Get(1, IR3_CHANNEL) >> 4);
  buffer[3] = 0xC3;//ADC_Get(0, CELL1_CHANNEL) >> 4;
  buffer[4] = 0xCD;//ADC_Get(0, CELL2_CHANNEL) >> 4;
  buffer[5] = 0xC7;//ADC_Get(0, CELL3_CHANNEL) >> 4;
}

void system_send(unsigned char num_packets, unsigned char **packets, unsigned char *IR_Cell)
{
  if (GDL_read(0) == 1)
  {
    unsigned char response = SSI_read_byte(0xAC);
    if (response == 0xAC)
    {
      system_IR_cell_add_packet(IR_Cell);
      int num_total_buffer = system_count_total_buffer(num_packets, packets);
      print_hex(num_total_buffer, 3);
      SSI_send_byte((char)((num_total_buffer & 0xFF00) >> 8));
      SSI_send_byte((char)(num_total_buffer & 0x00FF));
      
      unsigned char i, j;
      for (i = 0; i < num_packets; ++i)
      {
        unsigned char *packet_i_th = packets[i];
        unsigned char packet_buffer_size = 6 * packet_i_th[0];
        SSI_send_byte(packet_i_th[0]);
        for (j = 1; j <= packet_buffer_size; ++j)
        {
          SSI_send_byte(packet_i_th[j]);
        }
      }

      for (i = 0; i < 6; ++i)
      {
        SSI_send_byte(IR_Cell[i]);
      }
    }
  }
}

void system_data_communication(unsigned char *buffer)
{
  //UART0_OutChar('a');
  unsigned char num_packets;
  unsigned char* packets[MAX_NUM_PACKETS_PER_ZERO];
  unsigned char *raw = (buffer + MAX_LIDAR_ROTATION_DATA);
  
  for (num_packets = 0; num_packets < MAX_NUM_PACKETS_PER_ZERO; ++num_packets)
  {
    //unsigned char *i_th_packet_start = buffer + (MAX_LIDAR_COMPUTED_PACKET_SIZE * i);
    packets[num_packets] = buffer + (MAX_LIDAR_COMPUTED_PACKET_SIZE * num_packets);
  }

  unsigned char packet_type = 0x00;
  
  num_packets = 0;
  while (packet_type != 0x03)
  {
    packet_type = lidar_get_packet(packets[num_packets], raw);
    if (packet_type == 0x00)
    {
      ++num_packets;
    }
    system_flush_buffer(raw, MAX_LIDAR_RAW_PACKET_SIZE);
  }
  
  // Data Collected
  //system_lidar_averaging(num_packets, packets);
  system_send(num_packets, packets, raw);
  
  // Must need this part to reserve the stack
  system_flush_buffer(buffer, MAX_LIDAR_DATA_SIZE);
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
  
  unsigned char buffer[MAX_LIDAR_DATA_SIZE] = { 0 };

  //uint32_t front_IR;
  
  while (1)
  {
    //system_IR_cell_add_packet(buffer);
    system_data_communication(buffer);
    //front_IR = ADC_Get(1, IR2_CHANNEL);
    //motor_testing(front_IR);
  }
  return 0;
}

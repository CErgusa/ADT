#include "system.h"

#include "SysTick (2).h"
#include "adc.h"
#include "SSI.h"
#include "UART1.h"
#include "Lidar.h"
#include "tm4c123gh6pm.h"
#include "GDL.h"

#define SSI0_CE_ON 0x0
#define SSI0_CE_OFF 0x1

void system_init(void)
{
	SSI_Init();     // SPI communication
	UART_Init();    // TeraTerm UART communication
	SysTick_Init(); // System timer
	ADC0_Init();    // Batter cell voltage
  ADC1_Init();    // IR sensors
	UART1_Init();   // Lidar UART Communication
	GDL_Init();     // General Data Logic
}

void system_send(int *buffer)
{
  // Send IR, Cell data
  // Only MSB 8bits
  buffer[MAX_PACKET_SIZE] = ADC_Get(1, IR1_CHANNEL) >> 4;
  buffer[MAX_PACKET_SIZE+1] = ADC_Get(1, IR2_CHANNEL) >> 4;
  buffer[MAX_PACKET_SIZE+2] = ADC_Get(1, IR3_CHANNEL) >> 4;
  buffer[MAX_PACKET_SIZE+3] = ADC_Get(0, CELL1_CHANNEL) >> 4;
  buffer[MAX_PACKET_SIZE+4] = ADC_Get(0, CELL2_CHANNEL) >> 4;
  buffer[MAX_PACKET_SIZE+5] = ADC_Get(0, CELL3_CHANNEL) >> 4;

  int lidar_split = 1;

  // Trigger GPIO interrupt of Rpi
  GDL_Send(0x01);

  int i = 0;
  while (i < 134)
  {
    // Lidar packet
    if (i < MAX_PACKET_SIZE)
    {
      SSI_send_byte((buffer[i] >> (8*(lidar_split--))) & 0xFF);
      if (lidar_split < 0)
      {
        ++i;
        lidar_split = 1;
      }
    }
    else // IR, battery cell data
    {
      SSI_send_byte(buffer[i]);
      ++i;
    }
  }
  // Reset GPIO interrupt
  GDL_Send(0x00);
}

// Jun change this as needed, this is our new main function
// I just wanted to move things towards an engine rather than
// always being in main!
int system_engine(void)
{
  stop_lidar();
  SysTick_Wait1us(10);
  int response = scan_lidar();

  if(response == RECEIVED)
  {
    //UART1_enableInterrupts();

    // get buffer for sample array
    int buffer[MAX_PACKET_SIZE+6] = {0};

    // init struct with noticable variables for debugger
    struct scan_node PacketHeader = { 0xAA, 0xAA, 0xAAAA, 0xAAAA };

    while (1)
    {
      // get all info from header
      get_packet_header(&PacketHeader);

      // get rest of the packet
      get_packet(buffer, &PacketHeader);

      // send the full packet
      system_send(buffer);
      
      // clear the buffer and reset the noticeable variables in scan_node
      reset_lidar_shit(buffer, &PacketHeader);
    }
  }

  return ERROR;
}

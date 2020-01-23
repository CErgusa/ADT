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
  // Send lidar buffer first
  int send_count = 0;
  int lidar_split = 1;

  // Send IR, Cell data
  unsigned char IR_CELL_MSB[6];
  // Only MSB 8bits

  //IR_CELL_MSB[0] = ADC_Get(1, IR1_CHANNEL) >> 4;
  //IR_CELL_MSB[1] = ADC_Get(1, IR2_CHANNEL) >> 4;
  //IR_CELL_MSB[2] = ADC_Get(1, IR3_CHANNEL) >> 4;
  //IR_CELL_MSB[3] = ADC_Get(0, CELL1_CHANNEL) >> 4;
  //IR_CELL_MSB[4] = ADC_Get(0, CELL2_CHANNEL) >> 4;
  //IR_CELL_MSB[5] = ADC_Get(0, CELL3_CHANNEL) >> 4;
  IR_CELL_MSB[0] = 11;
  IR_CELL_MSB[1] = 22;
  IR_CELL_MSB[2] = 33;
  IR_CELL_MSB[3] = 44;
  IR_CELL_MSB[4] = 55;
  IR_CELL_MSB[5] = 66;
  
  // Check whether the Rpi is ready
  int SSI_CE_PA3 = (GPIO_PORTA_DATA_R & 0x08) >> 3;
  
  // If not, wait until it is ready  
  while (SSI_CE_PA3 == SSI0_CE_OFF)
  {
    SSI_CE_PA3 = (GPIO_PORTA_DATA_R & 0x08) >> 3;
  }
  
  // First sending
  while (send_count < 6)
  {
    // Wait next transfer from the master
    if (SSI_CE_PA3 == SSI0_CE_ON)
    {
      SSI_send_byte(IR_CELL_MSB[send_count]);
      ++send_count;
    }
    SSI_CE_PA3 = (GPIO_PORTA_DATA_R & 0x08) >> 3;
  }

//  // First sending
//  SSI_send_byte((buffer[send_count] >> (8*(lidar_split--))) & 0xFF);
//  while (send_count < (MAX_PACKET_SIZE+6))
//  {
//    SSI_CE_PA3 = (GPIO_PORTA_DATA_R & 0x08) >> 3;
//    // Wait next transfer from the master
//    if (SSI_CE_PA3 == SSI0_CE_ON)
//    {
//      if (send_count < MAX_PACKET_SIZE)
//      {
//        SSI_send_byte((buffer[send_count] >> (8*(lidar_split--))) & 0xFF);
//        if (lidar_split < 0)
//        {
//          ++send_count;
//          lidar_split = 1;
//        }
//      }
//      else
//      {
//        SSI_send_byte(IR_CELL_MSB[send_count-MAX_PACKET_SIZE]);
//        ++send_count;
//      }
//    }
//  }
}



// Jun change this as needed, this is our new main function
// I just wanted to move things towards an engine rather than
// always being in main!
int system_engine(void)
{
  // while (1) {SSI_in();}
	stop_lidar();
	SysTick_Wait1us(10);
	int response = scan_lidar();

	if(response == RECEIVED)
	{
    //UART1_enableInterrupts();

		// get buffer for sample array
		int buffer[MAX_PACKET_SIZE];
    int i = 0;
    for (i = 0; i < MAX_PACKET_SIZE; ++i)
    {
      buffer[i] = 0xABCD;
    }
			// init struct with noticable variables for debugger
		struct scan_node PacketHeader = { 0xAA, 0xAA, 0xAAAA, 0xAAAA };

    while (1)
    {
				// get all info from header

			//get_packet_header(&PacketHeader);
				
				// get rest of the packet
			//get_packet(buffer, &PacketHeader);			
			
			// send the full packet
      //system_send(buffer);
      
			// clear the buffer and reset the noticable variables in scan_node
			//reset_lidar_shit(buffer, &PacketHeader);
            
      // send fake packet
      system_send(buffer);
    }
	}

	return ERROR;
}

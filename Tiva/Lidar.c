#include "Lidar.h"
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "SysTick (2).h"

#define pi  3.14159f

int check_scan_response(void);

// PB0: UART1 Rx <- Lidar Tx
// PB1: UART1 Tx -> Lidar Rx
// PF2 -> M_SCTP(lidar): M1PWM6
// PF3 -> DEV_EN(lidar)
// PF4 -> M_EN(lidar)
void PortF_Init(void)
{
  // Activate clock for Port F
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGC2_GPIOF;
  
  // Wait until Port F is ready
  while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R5) == 0){};

  // Set input/output
  // set PF4-2 as output
  // set [4:3]
	GPIO_PORTF_DIR_R |= 0x18;
	
  // Alt function
  // Enable alt function: set [2]
  GPIO_PORTF_AFSEL_R |= 0x04;
  // Disable alt function: clear [4:3]
  GPIO_PORTF_AFSEL_R &= ~(0x18);
  
  // Configure the PMCn fields in the GPIOPCT
  GPIO_PORTF_PCTL_R |= GPIO_PCTL_PF2_M1PWM6;
  
  // Disable anlog function
  // clear [4:2]
  GPIO_PORTF_AMSEL_R &= ~0x1C;
    
  // Drive Strength: 8mA
  // set [4:3]
  GPIO_PORTF_DR8R_R |= 0x18;
  
  // Disable Open Drain
  // clear [4:3]
  GPIO_PORTF_ODR_R &= ~(0x18);  
    
  // Disable pull-up
  // clear [4:3]
  GPIO_PORTF_PUR_R &= ~(0x18);
    
  // Enable pull-down
  // set [4:3]
  GPIO_PORTF_PDR_R |= 0x18;
  
  // Enable digital function
  // set [4:2]
	GPIO_PORTF_DEN_R |= 0x1C;
}


void Lidar_Init(void)
{
  SYSCTL_RCGC0_R |= SYSCTL_RCGC0_PWM0;
  PWM1_CTL_R = 0x00000000;//Disable PWM
  PWM1_0_GENA_R |= 0x000000C8;//Drive PWMa High,Action for Comparator is drive high
  PWM1_0_LOAD_R |= 0x00186A00;//Load 1600000 for 6 Hz frequency so that we can check blinking of LED.
  PWM1_0_CMPA_R |= 0x00000063;//25% Duty Cycle
  PWM1_0_CTL_R |= 0x00000001;//Start Timer for PWM Generator
  PWM1_ENABLE_R |= 0x00000001;//Start PWM  
}


int scan_lidar()
{
		// scan command [A5 60]
	UART1_OutChar(0xA5);
	UART1_OutChar(0x60);
	
	return check_scan_response();
}

int check_scan_response()
{
    int response = (int)UART1_InChar();
    while(response != SCAN_RESPONSE_HEADER)
    {
      response = (int)UART1_InChar();
    }
    if(response == SCAN_RESPONSE_HEADER)
    {
        int find_packet = 0;
        int scan_response[SCAN_HEADER_SIZE] = { 0, 0, 0, 0, 0, 0, 0 };
  
        int counter = 0;
        scan_response[counter++] = response;
        
        // debug: make sure the packet is as follows
        // A5 5A 05 00 00 40 81
        while(find_packet != SCAN_BYTE6)
        {
            scan_response[counter] = UART1_InChar();
            find_packet = scan_response[counter++];
        }

        if(scan_response[0] == SCAN_BYTE0 && scan_response[1] == SCAN_BYTE1 &&
					 scan_response[2] == SCAN_BYTE2 && scan_response[3] == SCAN_BYTE3 &&
   	       scan_response[4] == SCAN_BYTE4 && scan_response[5] == SCAN_BYTE5 &&
           scan_response[6] == SCAN_BYTE6)
        {
            return RECEIVED;
        }
        else
        {
            return NOT_RECEIVED;
        }
    }

    return NOT_RECEIVED;
}

void get_packet_header(struct scan_node * PacketHeader)
{
	// check first bit of packet header is correct
	int response = (int)UART1_InChar();
	while(response != PACKET_HEADER_FIRST)
	{
		response = (int)UART1_InChar();
	}
	
	// check second bit of packet header is correct
	response = (int)UART1_InChar();	
	if(response == PACKET_HEADER_SECOND)
	{		
		for(int i = 0; i < 4; ++i)
		{
			char buffer[2];
			switch(i)
			{
				case 0: PacketHeader->packet_type = (int)UART1_InChar();
								break;
				
				case 1: PacketHeader->sample_quantity = (int)UART1_InChar();
								break;
				
				case 2:	buffer[0] = (int)UART1_InChar(); // first 8 bits
								buffer[1] = (int)UART1_InChar(); // second 8 bits
								// 16-bit value, need to make it one
								PacketHeader->start_angle = buffer[0] | buffer[1] << 8;
								break;
				
				case 3: buffer[0] = (int)UART1_InChar(); // first 8 bits
								buffer[1] = (int)UART1_InChar(); // second 8 bits
								// 16-bit value, need to make it one
								PacketHeader->ending_angle = buffer[0] | buffer[1] << 8;
								break;
			}
		}
	}
}

void get_packet(int * buffer, struct scan_node * PacketHeader)
{
	for(int i = 0; i < SCAN_NODE_OFFSET; ++i)
	{
		switch(i)
			{
				case 0: buffer[0] = PACKET_HEADER;
								break;
				
				case 1: buffer[1] = PacketHeader->packet_type;
								break;
				
				case 2: buffer[2] = PacketHeader->sample_quantity - 1;
								break;
				
				case 3:	buffer[3] = PacketHeader->start_angle;
								break;
				
				case 4: buffer[4] = PacketHeader->ending_angle;
								break;
			}
	}
	
	for(int i = SCAN_NODE_OFFSET; i < PacketHeader->sample_quantity + SCAN_NODE_OFFSET; ++i)
	{
		if(i == SCAN_NODE_OFFSET)
			buffer[i] = DOODBEEF;
		else
			buffer[i] = ((int)UART1_InChar());
	}
	buffer[PacketHeader->sample_quantity + SCAN_NODE_OFFSET] = DEADBEEF;
}
	
void reset_lidar_shit(int * buffer, struct scan_node * PacketHeader)
{
	for(int i = 0; i < MAX_PACKET_SIZE; ++i)
	{
			buffer[i] = 0;
	}
	
	PacketHeader->packet_type = 0xAA;
	PacketHeader->sample_quantity = 0xAA;
	PacketHeader->start_angle = 0xAAAA;
	PacketHeader->ending_angle = 0xAAAA;
}

void restart_lidar()
{
		// restart command [A5 40]
	UART1_OutChar(0xA5);
	UART1_OutChar(0x40);
}
void stop_lidar()
{
	// stop command [A5 65]
	UART1_OutChar(0xA5);
	UART1_OutChar(0x65);
}

// PC4 -> M_SCTP(lidar)
// PC5 -> M_EN(lidar)
void set_lidar_speed(void)
{
  // If negative speed, it means default
  //if (speed_percent < 0)
  //{
    // clear enable bit(PC5)
  //  GPIO_PORTC_DATA_R &= ~(0x20);
  //}
  // To control motor, set M_EN(PC5) high
  // Generate 10kHz sqaure wave..
}

void health()
{
	// health command [A5 91]
	UART1_OutChar(0xA5);
	UART1_OutChar(0x91);

}

void device_status()
{
		// device info [A5 90]
	UART1_OutChar(0xA5);
	UART1_OutChar(0x90);
}

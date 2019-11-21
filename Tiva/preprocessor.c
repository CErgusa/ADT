#include "SSI.h"
#include "adc.h"
#include "preprocessor.h"

void PP_Get(int data)
{
  static int counter = 0;
	
	  // if the next packet is coming in
	if(data == PACKET_FIRST_BYTE)
	{
		// reset the buffer
		PP_Send(data);
		counter = 0;
	}
	else
	{
		// place the data in the packet
		packet_to_send[counter] = data;
		++counter;
	}
}

void PP_Send(int data)
{
	// send packet_to_send to RPi
	// TODO: -^
	
	
	// refresh	
	clear_pts();              // clear all data from packet_to_send
	packet_to_send[0] = data; // put new data in the packet_to_send
}

void clear_pts(void)
{
	for(int i = 0; i < MAX_PACKET_SIZE; ++i)
	{
		packet_to_send[i] = END_OF_PACKET;
	}
}

#include "lidar.h"
#include "systick.h"
#include "UART.h"
#include <stdint.h> // uint_32
#include "tm4c123gh6pm.h"
#include <math.h>

#define SCAN_BYTE0 0xA5
#define SCAN_BYTE1 0x5A
#define SCAN_BYTE2 0x05
#define SCAN_BYTE3 0x00
#define SCAN_BYTE4 0x00
#define SCAN_BYTE5 0x40
#define SCAN_BYTE6 0x81

#define MAX_LIDAR_SAMPLES 0x28

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


void lidar_init(void)
{
  //SYSCTL_RCGC0_R |= SYSCTL_RCGC0_PWM0;
  //PWM1_CTL_R = 0x00000000;//Disable PWM
  //PWM1_0_GENA_R |= 0x000000C8;//Drive PWMa High,Action for Comparator is drive high
  //PWM1_0_LOAD_R |= 0x00186A00;//Load 1600000 for 6 Hz frequency so that we can check blinking of LED.
  //PWM1_0_CMPA_R |= 0x00000063;//25% Duty Cycle
  //PWM1_0_CTL_R |= 0x00000001;//Start Timer for PWM Generator
  //PWM1_ENABLE_R |= 0x00000001;//Start PWM
}


void lidar_scan_command(void)
{
		// scan command [A5 60]
	UART1_OutChar(0xA5);
	UART1_OutChar(0x60);
}

int lidar_scan_response(void)
{
  unsigned char scan_response[SCAN_HEADER_SIZE] = { 0 };
  do
  {
    lidar_scan_command();
  
    scan_response[0] = UART1_InChar();
  
    while (scan_response[0] != SCAN_BYTE0)
    {
      scan_response[0] = UART1_InChar();
    }
  
    int i;  
    for (i = 1; i < SCAN_HEADER_SIZE; ++i)
    {
      scan_response[i] = UART1_InChar();
    }
  }
  while( (scan_response[0] != SCAN_BYTE0) ||  // 0xA5
         (scan_response[1] != SCAN_BYTE1) ||  // 0x5A
         (scan_response[2] != SCAN_BYTE2) ||  // 0x05
         (scan_response[3] != SCAN_BYTE3) ||  // 0x00
         (scan_response[4] != SCAN_BYTE4) ||  // 0x00
         (scan_response[5] != SCAN_BYTE5) ||  // 0x40
         (scan_response[6] != SCAN_BYTE6));   // 0x81
  return 1;
}


void lidar_push_filtered_to_buffer(int *angle_filtered, int *dist_filtered, unsigned char *buffer)
{
  int filtered_size = buffer[0];
  int i;
  for (i = 1; i < filtered_size; ++i)
  {
    int index = (2 * i) - 1;
    // MSB 8 bit
    buffer[index] = angle_filtered[filtered_size] >> 8;
    // LSB 8
    buffer[index+1] = (angle_filtered[filtered_size] & 0x00FF);

    buffer[filtered_size + index] = dist_filtered[filtered_size] >> 8;
    buffer[filtered_size + index + 1] = (dist_filtered[filtered_size] & 0x00FF);
  }
}

void lidar_calculate_angle_distance(unsigned char *raw, unsigned char *buffer)
{
  unsigned char lsn = raw[3]; // ~0x28
  int fsa = (raw[4] | (raw[5] << 8));
  int lsa = (raw[6] | (raw[7] << 8));

  int distance[MAX_LIDAR_SAMPLES] = { 0 };
  float ang_correct[MAX_LIDAR_SAMPLES] = { 0.f };
  int angle_filtered[MAX_LIDAR_SAMPLES] = { 0 };
  int dist_filtered[MAX_LIDAR_SAMPLES] = { 0 };

  float diff_angle = 0.0f;
  float starting_angle = 0.0f;
  float ending_angle = 0.f;
  float mid_angle = 0.0f;

  int i;
  int filtered_size = 0;

  for (i = 0; i < lsn;)
  {
    int i_th_sample = (2 * i) + 10; // (10,11), (12,13), (14,15)....
    int raw_dist = (raw[i_th_sample] | (raw[i_th_sample+1] << 8));
    if (raw_dist == 0)
    {
      distance[i] = 0;
      ang_correct[i] = 0.f;
    }
    else
    {
      distance[i] = raw_dist >> 2; // Distance = raw / 4
      float dist_float = (float)(distance[i]);
      float paramter = 21.8f * ((155.3f - dist_float) / (155.3f * dist_float));
      ang_correct[i] = atan(paramter);
    }

    if (i == 0) // First sample: Starting angle, distance
    {
      //starting_angle =(((float)(fsa >> 1)) / 64.f) + ang_correct[0];
      starting_angle = ((float)(fsa >> 7)) + ang_correct[0];
      i = lsn - 1; // Get the last one next
      if (starting_angle >= 270.f || starting_angle <= 90.f)
      {
        angle_filtered[filtered_size] = (int)(starting_angle * 100.f);
        dist_filtered[filtered_size] = distance[0];
        ++filtered_size;
      }
    }
    else if (i == lsn - 1) // Last sample: Ending angle, distance
    {
      //ending_angle = (((float)(lsa >> 1)) / 64.f) + ang_correct[lsn-1];
      ending_angle = ((float)(lsa >> 7)) + ang_correct[lsn-1];
      diff_angle = ending_angle - starting_angle;
      
      i = 1; // Now get middle samples
    }
    else // Middle samples
    {
      mid_angle = ((diff_angle / ((float)(lsn-1))) * (float)(i-1)) + starting_angle + ang_correct[i];
      if (mid_angle >= 270.f || mid_angle <= 90.f)
      {
        angle_filtered[filtered_size] = (int)(mid_angle * 100.0f);
        dist_filtered[filtered_size] = distance[i];
        ++filtered_size;
      }

      if (i == lsn - 2) // Loop terminates
      {
        if (ending_angle >= 270.f || ending_angle <= 90.f)
        {
          angle_filtered[filtered_size] = (int)(ending_angle * 100.f);
          dist_filtered[filtered_size] = distance[lsn-1];
          ++filtered_size;
        }
        break;
      }
      else
      {
        ++i;
      } 
    }
  }

  buffer[0] = filtered_size;
  lidar_push_filtered_to_buffer(angle_filtered, dist_filtered, buffer);
}

int lidar_get_packet(unsigned char *buffer)
{
  int i, j;
  unsigned char raw[MAX_LIDAR_PACKET_SIZE] = { 0 };

  for (i = 0; i < 10; ++i)
  {
    raw[i] = UART1_InChar();
  }

  if (raw[0] == 0xAA && // Valid PH:LL
      raw[1] == 0x55 && // Valid PH:HH
      raw[2] == 0x00)   // Only if point cloud packet
  {
    int sample_quantity = raw[3];

    for(j = 0; j < (sample_quantity*2); ++i, ++j)
    {
      raw[i] = UART1_InChar();
    }

    lidar_calculate_angle_distance(raw, buffer);
    return 1;
  }
  else
  {
    return 0;
  }
}

void restart_lidar()
{
		// restart command [A5 40]
	UART1_OutChar(0xA5);
	UART1_OutChar(0x40);
}

void lidar_stop_command(void)
{
	// stop command [0xA5 0x65]
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
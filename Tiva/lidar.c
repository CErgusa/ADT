#include "lidar.h"
#include "systick.h"
#include "UART.h"
#include <stdint.h> // uint_32
#include "tm4c123gh6pm.h"
#include <math.h>
#include "utils.h"

#define SCAN_BYTE0 0xA5
#define SCAN_BYTE1 0x5A
#define SCAN_BYTE2 0x05
#define SCAN_BYTE3 0x00
#define SCAN_BYTE4 0x00
#define SCAN_BYTE5 0x40
#define SCAN_BYTE6 0x81

#define MAX_LIDAR_SAMPLES 0x28
#define SCAN_HEADER_SIZE 0x07

#define LEFT_EDGE_DEGREE 270.f
#define RIGHT_EDGE_DEGREE 90.f
#define LEFT_EDGE_RAD 4.712f
#define RIGHT_EDGE_RAD 1.570f

#define TWOPI 6.2831f

// PB0: UART1 Rx <- Lidar Tx
// PB1: UART1 Tx -> Lidar Rx
// PF2 -> M_SCTP(lidar): M1PWM6
// PF3 -> DEV_EN(lidar)
// PF4 -> M_EN(lidar)
char packet_count = 0;

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

void lidar_sync_packet(unsigned char *buffer)
{
  
}


void lidar_push_to_buffer(float *theta_, int r_, unsigned char *buffer)
{
// | Size | theta | r    |
//   1char  4char   2char
//   1    + 160   + 80  
  buffer[0] += 1;
  unsigned char n = buffer[0]; // 1~0x28
  unsigned char theta_index = (4 * (n-1)) + 1;
  unsigned char r_index = (2 * (n-1)) + 161;
  
  store_float_in_4char(theta_, &buffer[theta_index]);
  
//  int IEEE_bits = 0;
//  IEEE_bits = *((int *)theta_);
//  buffer[theta_index]   = (IEEE_bits & 0xFF000000) >> 24;
//  buffer[theta_index+1] = (IEEE_bits & 0x00FF0000) >> 16;
//  buffer[theta_index+2] = (IEEE_bits & 0x0000FF00) >> 8;
//  buffer[theta_index+3] = (IEEE_bits & 0x000000FF) >> 0;
  
  buffer[r_index] = (r_ >> 8);
  buffer[r_index+1] = (r_ & 0x00FF);
}

void lidar_calculate_angle_distance(unsigned char *raw, unsigned char *buffer)
{
  //UART0_OutChar('c');
  unsigned char lsn = raw[3]; // 0 ~ 0x28
  int fsa = (raw[4] | (raw[5] << 8));
  int lsa = (raw[6] | (raw[7] << 8));

  float diff_angle = 0.0f;
  float starting_angle = 0.0f;
  float ending_angle = 0.0f;

  unsigned char i;
  print_hex(lsn, 2);
  UART0_OutChar(' ');
  
  for (i = 0; i < lsn;)
  {
    // (10,11), (12,13), (14,15) .... (88,89)
    unsigned char i_th_sample = (2 * i) + 10;
    int raw_dist = (raw[i_th_sample] | (raw[i_th_sample+1] << 8));
    float distance_i = 0.0f;
    float angle_i = 0.0f;
    float ang_correct_i = 0.0f;
    float angle_xcorrect_i_degree = 0.0f;
    
    // Compute the ith distance and angcorrect
    if (raw_dist == 0)
    {
      distance_i = 0.f;
      ang_correct_i = 0.f;
    }
    else
    {
      distance_i = ((float)raw_dist) / 4.0f; // Distance = raw / 4
      float parameter = 21.8f * ((155.3f - distance_i) / (155.3f * distance_i));
      ang_correct_i = atan(parameter); // return radian
    }

    // First sample: Starting angle, distance
    if (i == 0)
    {
      if (fsa == 0x00)
      {
        angle_i = 0.0f;
      }
      else
      {
        angle_xcorrect_i_degree = (((float)(fsa >> 1)) / 64.f);
        angle_i = (radian(angle_xcorrect_i_degree)) + ang_correct_i;
      }

      // Keep the first angle for computing difference
      starting_angle = angle_i;
      float_angle_debugging_degree(angle_i);
    }
    else if (i == lsn - 1) // Last sample: Ending angle, distance
    {
      if (lsa == 0x00)
      {
        angle_i = 0.0f;
        diff_angle = 0.0f;
      }
      else
      {
        angle_xcorrect_i_degree = (((float)(lsa >> 1)) / 64.f);
        angle_i = (radian(angle_xcorrect_i_degree)) + ang_correct_i;
      }
      
      if (angle_i < starting_angle)
      {
        diff_angle = (angle_i + TWOPI) - starting_angle;
      }
      else
      {
        diff_angle = angle_i - starting_angle;
      }
      
      float_angle_debugging_degree(angle_i);
      ending_angle = angle_i;
    }
    else // Middle samples: i = [1 ... lsn-1]
    {
      angle_i = ((diff_angle / ((float)(lsn-1))) * (float)(i-1)) 
                 + starting_angle + ang_correct_i;
    }

    int r_i = (int)(distance_i);
    
    if (i != lsn - 1)
    {
      lidar_push_to_buffer(&angle_i, r_i, buffer);
    }
    
    // Update the counter
    if (i == 0)
    {
      UART0_OutChar(' ');
      if (fsa == 0)
      {
        if (lsa == 0)
        {
          break; // FSA = LSA = 0: zero angle data
        }
      }
      
      if (lsn == 0x01) // Only one sample
      {
        break;
      }
      i = lsn - 1; // Get the last one next
    }
    else if (i == lsn - 1) // Last sample: Ending angle, distance
    {
      if (lsn == 2)
      {
        // No middle samples,
        break;
      }
      i = 1;
    }
    else if (i == lsn - 2) // Loop terminates
    {
      lidar_push_to_buffer(&ending_angle, r_i, buffer);
      break;
    }
    else
    {
      ++i;
    }
  }
  // If no data in the range, just skip
  //UART0_OutChar('d');
}

unsigned char lidar_get_packet(unsigned char *buffer, unsigned char *raw)
{
  //UART0_OutChar('b');
  unsigned char i, j;
  
  while (raw[0] != 0xAA)
  {
    raw[0] = UART1_InChar();
  }
  
  for (i = 1; i < 10; ++i)
  {
    raw[i] = UART1_InChar();
  }

  UART0_OutChar('<');
  unsigned char packet_type = 0;
  if (raw[0] == 0xAA && // Valid PH:LL
      raw[1] == 0x55)   // Valid PH:HH
  {
    unsigned char sample_quantity = raw[3] * 2; // 40 * 2 = 80

    for (j = 0; j < sample_quantity; ++i, ++j)
    {
      raw[i] = UART1_InChar();
    }
    
    if (raw[2] == 0x00) // point Cloud
    {
      if (raw[3] <= 0x28) // less than maximum
      {
        if (raw[3] != 0x00) // non-Zero data
        {
          lidar_calculate_angle_distance(raw, buffer);
          ++packet_count;
          packet_type = 0x00; // point cloud
        }
        else // zero quantity samples
        {
          UART0_OutChar('0');
          packet_type = 0x1;
        }
      }
      else // greater than maximum data
      {
        UART0_OutChar('G');
        packet_type = 2;
      }
    }
    else // Zero Packet
    {
      UART0_OutChar('Z');
      print_hex(packet_count, 2);
      packet_count = 0;
      packet_type = 3; // Zero Packet
    }
  }
  else
  {
    UART0_OutChar('F');
    packet_type = 4; // Failed
  }
  UART0_OutChar('>');
  //UART0_OutChar('e');
  return packet_type;
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

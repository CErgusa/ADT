/******************************************************************************/
/*!
\file   motor_control.h
\author Kenyu Tatsuta
\par    email: k.tatsuta\@digipen.edu
\par    DigiPen login: k.tatsuta
\par    Course: ECE360
\par    3rd Year Project
\brief  
  This is the header file for all member functions
  to initiate motor control.  
  
*/
/******************************************************************************/

//void portB_Init(void);
//void portE_Init(void);
#include <stdint.h>

#define FORWARD 1
#define BACKWARD 0

		//Initialization function for motors
void motor_control_Init(void);

//-------------------------------------------------------------------------------------------------

		// Modifies PB6 to output PWM signal to control left motor speed
void PWM_left_Init(uint16_t period, uint16_t high);

		// Modifies PB7 to output PWM signal to control right motor speed
void PWM_right_Init(uint16_t period, uint16_t high);

//-------------------------------------------------------------------------------------------------

		// Modifies PE4 to output digital output to control left motor direction
void Dir_left_Init(void);

		// Modifies PE5 to output digital output to control right motor direction
void Dir_right_Init(void);

//-------------------------------------------------------------------------------------------------

		// Modifies PB2 as interrupt at positive edge of left motor encoder A
void Encoder_left_A_Init(void);

		// Modifies PB3 as interrupt at positive edge of left motor encoder B
void Encoder_left_B_Init(void);

		// Modifies PB4 as interrupt at positive edge of right motor encoder A
void Encoder_right_A_Init(void);

		// Modifies PB5 as interrupt as positive edge of right motor encoder B
void Encoder_right_B_Init(void);

//-------------------------------------------------------------------------------------------------

		// Sets the speed of the right motor by modifying PB7's PWM duty cycle duration
void set_right_speed(int speed);

		// Sets the speed of the left motor by modifying PB6's PWM duty cycle duration
void set_left_speed(int speed);

		// Sets the direction of the right motor by changing digital signal output of PE5
void set_right_dir(int direction);

		// Sets the direction of the left motor by changing digital signal output of PE4
void set_left_dir(int direction);

		// Gradually changes the speed of left motor until it reaches desired speed
void change_left_speed(int speed);

		// Gradually changes the speed of right motor until it reaches desired speed
void change_right_speed(int speed);

//-------------------------------------------------------------------------------------------------

		// Called outside motor_control.c when getting the total left encoder data
unsigned long int Get_LeftEncoder(void);

		// Called outside motor_control.c when getting the total right encoder data
unsigned long int Get_RightEncoder(void);

		// Called outside motor_control.c when resetting the encoder datas
void Reset_Encoders(void);

//-------------------------------------------------------------------------------------------------

		// Include this function in an infinite loop (while(1)) so that
		// it can test motor control functions (keeping it straight, turning, etc.
void motor_testing(uint32_t front_IR);

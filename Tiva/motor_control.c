/******************************************************************************/
/*!
\file   motor_control.cpp
\author Kenyu Tatsuta
\par    email: k.tatsuta\@digipen.edu
\par    DigiPen login: k.tatsuta
\par    Course: ECE360
\par    3rd Year Project
\brief  
  This is the implementation file for all member functions
  for motor controls, as specified in specification
  file motor_control.h.  
  
  Operations include:

  - Function that calls all initialization functions
  - PWM initialization for left motor speed control
  - PWM initialization for right motor speed control
  - GPIO output initializaition for left motor direction
  - GPIO output initializaition for right motor direction
  - GPIO input initialization for left motor encoder A detection
  - GPIO input initialization for left motor encoder B detection
  - GPIO input initialization for right motor encoder A detection
  - GPIO input initialization for right motor encoder B detection
  - Interrupt function when left motor encoder A is detected
  - Interrupt function when left motor encoder B is detected
  - Interrupt function when right motor encoder A is detected
  - Interrupt function when right motor encoder B is detected
  - Function that sets the speed of left motor
  - Function that sets the speed of right motor
	- Function that sets the direction of the left motor
	- Function that sets the direction of the right motor
  - Function that changes gradually the speed of left motor
  - Function that changes gradually the speed of right motor
	- Gives total number of left encoder tics (A + B)
	- Gives total number of right encoder tics (A + B)
	- Resets the read encoder data to 0
	- Function that is called to test motor control functions

*/
/******************************************************************************/

#include "motor_control.h"
#include "tm4c123gh6pm.h"

volatile uint32_t left_A_count;		// Incremented when interrupt for left encoder A detector is called (PB2)
volatile uint32_t left_B_count;		// Incremented when interrupt for left encoder B detector is called (PB3)
volatile uint32_t right_A_count;	// Incremented when interrupt for right encoder A detector is called (PB4)
volatile uint32_t right_B_count;	// Incremented when interrupt for right encoder B detector is called (PB5)
long int counter = 0;		// counter for motor_testing


/*
	Include this function to the main function (or function that calls all 
	initialization functions) to modify the pins on Tiva TM4C123GH6PM to 
	control the motors of the tank. 
*/
void motor_control_Init(void)
{
  // with clock speed 16MHz
  // period 10000 ~= 1.6kHz
  // period 65000 ~= 250Hz | w/ other codes ~= 1.23 kHz
  // period 32500 ~= 500Hz | w/ other codes ~= 2.45 kHz
  uint16_t period = 65000; //freq = clock/period
  uint16_t high = 2;

  PWM_left_Init(period, high);
  PWM_right_Init(period, high);
  Dir_left_Init();
  Dir_right_Init();
  Encoder_left_A_Init();
  Encoder_left_B_Init();
  Encoder_right_A_Init();
  Encoder_right_B_Init();

  set_right_speed(50);
  set_left_speed(50);
  set_right_dir(FORWARD);
  set_left_dir(FORWARD);
	
	counter = 0;
}



//-------------------------------------------------------------------------------------------------
// PB6(left) and PB7(right)
// for output PWM motor speed control
//PG650 OF DATASHEET NEEDED FOR MUX TABLE
//pg 737 for timer0 control

// PB6 (Left motor speed control) | Timer0A
/*
	This function will modify pin PB6 to output PWM signal
	using internal timers. PB6 will be used to control the 
	speed of the left motor. PB6 uses Timer0A as its internal
	timer.

  \param period
		int that determines the frequency of the PWM

	\param high
		int that determines how long PWM signal stays high

	\attention
		frequency = clock/period. high is low in this one
		so it doesn't start moving after the initiation
		is done.
*/
void PWM_left_Init(uint16_t period, uint16_t high)
{
  SYSCTL_RCGCTIMER_R |= 0x01;			//activate timer0
  SYSCTL_RCGCGPIO_R |= 0x02;			//activate portB
  while ((SYSCTL_PRGPIO_R & 0x02) == 0) {};
  GPIO_PORTB_AMSEL_R &= ~(0x40);	//disable analog function at PB6
  GPIO_PORTB_AFSEL_R |= 0x40;			//enable alt func on PB6
  GPIO_PORTB_DEN_R |= 0x40;				//enable digital I/O on PB6
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xF0FFFFFF) + 0x07000000;
				//disable digital function of PB6 and activate timer0
  TIMER0_CTL_R &= ~0x00000001;		//disable timer0A during setup
  TIMER0_CFG_R = 0x00000004; 			//configure for 16-bit timer mode
  TIMER0_TAMR_R = 0x0000000A; 		//PWM and periodic mode
  TIMER0_TAILR_R = period - 1;		//timer start value
  TIMER0_TAMATCHR_R = period - high - 1;	// duty cycle = high/period
  TIMER0_CTL_R |= 0x00000001;			//enable timer0A 16-b, PWM
}
// PB7 (Right motor speed control) | Timer0B
/*
	This function will modify pin PB7 to output PWM signal
	using internal timers. PB7 will be used to control the 
	speed of the right motor. PB7 uses Timer0B as its internal
	timer.

  \param period
		int that determines the frequency of the PWM

	\param high
		int that determines how long PWM signal stays high

	\attention
		frequency = clock/period. high is low in this one
		so it doesn't start moving after the initiation
		is done.
*/
void PWM_right_Init(uint16_t period, uint16_t high)
{
  SYSCTL_RCGCTIMER_R |= 0x01;			//activate timer0
  SYSCTL_RCGCGPIO_R |= 0x02;			//activate portB
  while ((SYSCTL_PRGPIO_R & 0x02) == 0) {};
  GPIO_PORTB_AMSEL_R = ~(0x80);		//disable analog function at PB7
  GPIO_PORTB_AFSEL_R |= 0x80;			//enable alt func on PB7
  GPIO_PORTB_DEN_R |= 0x80;				//enable digital I/O on PB7
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0x0FFFFFFF) + 0x70000000;
				//disable digital function of PB7 and activate timer0
  TIMER0_CTL_R &= ~0x00000100;		//disable timer0B during setup
  TIMER0_CFG_R = 0x00000004; 			//configure for 16-bit timer mode
  TIMER0_TBMR_R = 0x0000000A; 		//PWM and periodic mode
  TIMER0_TBILR_R = period - 1;		//timer start value
  TIMER0_TBMATCHR_R = period - high - 1;	// duty cycle = high/period
  TIMER0_CTL_R |= 0x00000100;			//enable timer0B 16-b, PWM
}



//-------------------------------------------------------------------------------------------------
// PE4(left) and PE5(right)
// for output GPIO motor direction control

// PE4 (left motor direction control)
/*
	This function will modify pin PE4 to output digital signal
	(so its either high (about 3.3V) or low (0V). PE4 will be 
	used to control the direction of the left motor.
*/
void Dir_left_Init(void)
{
  SYSCTL_RCGC2_R |= 0x10;						// Activate clock for Port E
  unsigned long delay = SYSCTL_RCGC2_R;
  GPIO_PORTE_LOCK_R = 0x4C4F434B;		// unlock GPIO Port E
  GPIO_PORTE_CR_R |= 0x10;					// allow changes to PE4 
  GPIO_PORTE_AMSEL_R &= ~(0x10); 		// disable analog on PE4
  GPIO_PORTE_DIR_R |= 0x10;					// PE4 out
  GPIO_PORTE_AFSEL_R &= ~(0x10);		// disable alt function PE4
  GPIO_PORTE_DEN_R |= 0x10;					// enable digital I/O on PE4
}
// PE5 (right motor direction control)
/*
	This function will modify pin PE5 to output digital signal
	(so its either high (about 3.3V) or low (0V). PE5 will be 
	used to control the direction of the right motor.
*/
void Dir_right_Init(void)
{
  SYSCTL_RCGC2_R |= 0x10;						// Activate clock for Port E
  unsigned long delay = SYSCTL_RCGC2_R;
  GPIO_PORTE_LOCK_R = 0x4C4F434B;		// unlock GPIO Port E
  GPIO_PORTE_CR_R |= 0x20;					// allow changes to PE5
  GPIO_PORTE_AMSEL_R &= ~(0x20); 		// disable analog on PE5
  GPIO_PORTE_DIR_R |= 0x20;					// PE5 out
  GPIO_PORTE_AFSEL_R &= ~(0x20);		// disable alt function PE5
  GPIO_PORTE_DEN_R |= 0x20;					// enable digital I/O on PE5
}



//-------------------------------------------------------------------------------------------------
// PB2(leftA) PB3(leftB) PB4(rightA) PB5(rightB)
// for input GPIO encoder data reading
// these have been modified to read positive edge


// PB2 (leftA) Timer3A | Interrupt #35 | PRI8 0x00FF.FFFF | EN1 0x0000.0008
/*
	This function will modify GPIO pin PB2 to call an 
	interrupt function, Timer3A_Handler, when it reads 
	positive edge. This pin will read the left motor's 
	encoder A output, and counts tics for each encoder
	rising edge.
*/
void Encoder_left_A_Init(void)
{
  DisableInterrupts();

  SYSCTL_RCGCTIMER_R |= 0x08; 		// activate timer3
  SYSCTL_RCGCGPIO_R |= 0x02;			// activate portB
  GPIO_PORTB_DEN_R |= 0x04;				// enable digital I/O on PB2
  GPIO_PORTB_AFSEL_R |= 0x04; 		// enable alt function on PB2
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFFF0FF) + 0x00000700;
				//disable digital function of PB2 and activate timer3
  TIMER3_CTL_R &= ~0x00000001;		// disable Timer3A during setup
  TIMER3_CFG_R = 0x00000004;			// configer for 16-bit timer mode
  TIMER3_TAMR_R = 0x00000007; 		// configure for input capture mode
  TIMER3_CTL_R &= ~(0x000C);			// TAEVENT is rising edge
  TIMER3_TAILR_R = 0x0000FFFF;		// start value
  TIMER3_IMR_R |= 0x00000004;			// enable capture match interrupt
  TIMER3_ICR_R |= 0x00000004;			// clear Timer3A capture flag
  TIMER3_CTL_R |= 0x00000001; 		// enable Timer3A
  NVIC_PRI8_R = (NVIC_PRI8_R & 0x00FFFFFF) | 0x40000000;
				// hold priority value 4 at interrupt 35
  NVIC_EN1_R |= 0x00000010;				// enable interrupt 35 in NVIC
  left_A_count = 0;

  EnableInterrupts();
}

// PB3 (leftB) Timer3B | Interrupt #36 | PRI9 0xFFFF.FF00 | EN1 0x0000.0010
/*
	This function will modify GPIO pin PB3 to call an 
	interrupt function, Timer3B_Handler, when it reads 
	positive edge. This pin will read the left motor's 
	encoder B output, and counts tics for each encoder
	rising edge.
*/
void Encoder_left_B_Init(void)
{
  DisableInterrupts();

  SYSCTL_RCGCTIMER_R |= 0x08; 		// activate timer3
  SYSCTL_RCGCGPIO_R |= 0x02;			// activate portB
  GPIO_PORTB_DEN_R |= 0x08;				// enable digital I/O on PB3
  GPIO_PORTB_AFSEL_R |= 0x08; 		// enable alt function on PB3
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFF0FFF) + 0x00007000;
				//disable digital function of PB3 and activate timer3
  TIMER3_CTL_R &= ~0x00000100;		// disable Timer3B during setup
  TIMER3_CFG_R = 0x00000004;			// configer for 16-bit timer mode
  TIMER3_TBMR_R = 0x00000007; 		// configure for input capture mode
  TIMER3_CTL_R &= ~(0x0C00);			// TBEVENT is rising edge
  TIMER3_TBILR_R = 0x0000FFFF;		// start value
  TIMER3_IMR_R |= 0x00000400;			// enable capture match interrupt
  TIMER3_ICR_R |= 0x00000400;			// clear Timer3B capture flag
  TIMER3_CTL_R |= 0x00000100; 		// enable Timer3B
  NVIC_PRI9_R = (NVIC_PRI9_R & 0xFFFFFF00) | 0x00000040;
				// hold priority value 4 at interrupt 36
  NVIC_EN1_R |= 0x00000008;				// enable interrupt 36 in NVIC

  left_B_count = 0;

  EnableInterrupts();
}

// PB4 (rightA) Timer1A | Interrupt #21 | PRI5 0xFFFF.00FF | EN0 0x0020.0000
/*
	This function will modify GPIO pin PB4 to call an 
	interrupt function, Timer1A_Handler, when it reads 
	positive edge. This pin will read the right motor's 
	encoder A output, and counts tics for each encoder
	rising edge.
*/
void Encoder_right_A_Init(void)
{
  DisableInterrupts();

  SYSCTL_RCGCTIMER_R |= 0x02; 		// activate timer1
  SYSCTL_RCGCGPIO_R |= 0x02;			// activate portB
  GPIO_PORTB_DEN_R |= 0x10;				// enable digital I/O on PB4
  GPIO_PORTB_AFSEL_R |= 0x10; 		// enable alt function on PB4
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFF0FFFF) + 0x00070000;
				//disable digital function of PB4 and activate timer1
  TIMER1_CTL_R &= ~0x00000001;		// disable Timer1A during setup
  TIMER1_CFG_R = 0x00000004;			// configer for 16-bit timer mode
  TIMER1_TAMR_R = 0x00000007; 		// configure for input capture mode
  TIMER1_CTL_R &= ~(0x000C);			// TAEVENT is rising edge
  TIMER1_TAILR_R = 0x0000FFFF;		// start value
  TIMER1_IMR_R |= 0x00000004;			// enable capture match interrupt
  TIMER1_ICR_R |= 0x00000004;			// clear Timer1A capture flag
  TIMER1_CTL_R |= 0x00000001; 		// enable Timer1A
  NVIC_PRI5_R = (NVIC_PRI5_R & 0xFFFF00FF) | 0x00004000;
				// hold priority value 4 at interrupt 21
  NVIC_EN0_R |= 0x00200000;				// enable interrupt 21 in NVIC

  right_A_count = 0;

  EnableInterrupts();
}

// PB5 (rightB) Timer1B | Interrupt #22 | PRI5 0xFF00.FFFF | EN0 0x0040.0000
/*
	This function will modify GPIO pin PB5 to call an 
	interrupt function, Timer1B_Handler, when it reads 
	positive edge. This pin will read the right motor's 
	encoder B output, and counts tics for each encoder
	rising edge.
*/
void Encoder_right_B_Init(void)
{
  DisableInterrupts();

  SYSCTL_RCGCTIMER_R |= 0x02; 		// activate timer1
  SYSCTL_RCGCGPIO_R |= 0x02;			// activate portB
  GPIO_PORTB_DEN_R |= 0x20;				// enable digital I/O on PB5
  GPIO_PORTB_AFSEL_R |= 0x20; 		// enable alt function on PB5
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFF0FFFFF) + 0x00700000;
				//disable digital function of PB5 and activate timer1
  TIMER1_CTL_R &= ~0x00000100;		// disable Timer1B during setup
  TIMER1_CFG_R = 0x00000004;			// configer for 16-bit timer mode
  TIMER1_TBMR_R = 0x00000007; 		// configure for input capture mode
  TIMER1_CTL_R &= ~(0x0C00);			// TAEVENT is rising edge
  TIMER1_TBILR_R = 0x0000FFFF;		// start value
  TIMER1_IMR_R |= 0x00000400;			// enable capture match interrupt
  TIMER1_ICR_R |= 0x00000400;			// clear Timer1B capture flag
  TIMER1_CTL_R |= 0x00000100; 		// enable Timer1B
  NVIC_PRI5_R = (NVIC_PRI5_R & 0x00FFFFFF) | 0x40000000;
				// hold priority value 4 at interrupt 22
  NVIC_EN0_R |= 0x00400000;				// enable interrupt 22 in NVIC

  right_B_count = 0;

  EnableInterrupts();
}

/* 
	Interrupt function called when PB2 reads positive edge.
*/
void Timer3A_Handler(void)
{
  TIMER3_ICR_R = 0x00000004;
  left_A_count++;		// Increment total number of left encoder A tics.
}
/* 
	Interrupt function called when PB3 reads positive edge.
*/
void Timer3B_Handler(void)
{
  TIMER3_ICR_R = 0x00000400;
  left_B_count++;		// Increment total number of left encoder B tics.
}
/*
  Interrupt function called when PB4 reads positive edge.
*/
void Timer1A_Handler(void)
{
  TIMER1_ICR_R = 0x00000004;
  right_A_count++;	// Increment total number of right encoder A tics.
}
/*
	Interrupt function called when PB5 reads positive edge.
*/
void Timer1B_Handler(void)
{
  TIMER1_ICR_R = 0x00000400;
  right_B_count++;	// Increment total number of right encoder B tics.
}


//-------------------------------------------------------------------------------------------------
// helper functions to change the speed and directions of the motors

/*
	This function will set the speed of the left motor between stop
	and full speed. This is achieved by changing the duty cycle
	of the PWM signal output from PB6. 

	\param speed
		any int from 5 to 95, the percentage speed
		of the motor to move. DO NOT input below 5 or above 95.
*/
void set_left_speed(int speed)
{
  uint16_t high = speed * 650;

  TIMER0_TAMATCHR_R = TIMER0_TAILR_R - high;
}

/*
	This function will set the speed of the right motor between stop
	and full speed. This is achieved by changing the duty cycle
	of the PWM signal output from PB7. 

	\param speed
		any int from 5 to 95, the percentage speed
		of the motor to move. DO NOT input below 5 or above 95.
*/
void set_right_speed(int speed)
{
  uint16_t high = speed * 650;

  TIMER0_TBMATCHR_R = TIMER0_TBILR_R - high;
}

/*
	This function will set the direction of the left motor between
	forward or backward. This is achieved by changing the digital
	output of PE4. 

	\param direction 
		takes either FORWARD or BACKWARD, declared at top as 1 or 0
*/
void set_right_dir(int direction)
{
  if (direction == BACKWARD)
    GPIO_PORTE_DATA_R |= 0x20;
  else
    GPIO_PORTE_DATA_R &= 0xDF;
}

/*
	This function will set the direction of the right motor between
	forward or backward. This is achieved by changing the digital
	output of PE5. 

	\param direction 
		takes either FORWARD or BACKWARD, declared at top as 1 or 0
*/
void set_left_dir(int direction)
{
  if (direction == FORWARD)
    GPIO_PORTE_DATA_R |= 0x10;
  else
    GPIO_PORTE_DATA_R &= 0xEF;
}

/*
	This function will gradually change the speed from current to
	desired speed. This is achieved by gradually changing the duty 
	cycle of the PWM signal output from PB6. 

	\param speed
		any int from 5 to 95, the percentage speed
		of the motor to move. DO NOT input below 5 or above 95.
*/
void change_left_speed(int speed)
{
  uint16_t high = speed * 650;

  uint16_t goal = TIMER0_TBILR_R - high;	// new, desired speed%

  if (TIMER0_TAMATCHR_R < goal)
  {
			// increment duty cycle if goal is faster
    for (int i = 0; i < goal - TIMER0_TAMATCHR_R; i++)
    {
      TIMER0_TAMATCHR_R += 1;
      for (int i = 0; i < 200; i++);
    }
  }
  else
  {
			// decrement duty cycle if goal is slower
    for (int i = 0; i < TIMER0_TAMATCHR_R - goal; i++)
    {
      TIMER0_TAMATCHR_R -= 1;
      for (int i = 0; i < 200; i++);
    }
  }

}

/*
	This function will gradually change the speed from current to
	desired speed. This is achieved by gradually changing the duty 
	cycle of the PWM signal output from PB7. 

	\param speed
		any int from 5 to 95, the percentage speed
		of the motor to move. DO NOT input below 5 or above 95.
*/
void change_right_speed(int speed)
{
  uint16_t high = speed * 650;

  uint16_t goal = TIMER0_TAILR_R - high;	// new, desired speed%

  if (TIMER0_TBMATCHR_R < goal)
  {
			// if goal is faster, increment
    for (int i = 0; i < goal - TIMER0_TBMATCHR_R; i++)
    {
      TIMER0_TBMATCHR_R += 1;
      for (int i = 0; i < 200; i++);
    }
  }
  else
  {
			// if goal is slower, decrement
    for (int i = 0; i < TIMER0_TBMATCHR_R - goal; i++)
    {
      TIMER0_TBMATCHR_R -= 1;
      for (int i = 0; i < 200; i++);
    }
  }
}


//-------------------------------------------------------------------------------------------------
// These function will be called outside motor_control.c

/*
	This function is called when client needs to know
	total number of tics the left motor encoder (A and B). 

	\return total 
		total number of left encoder tics read by the tiva
*/
unsigned long int Get_LeftEncoder(void)
{
	return (left_A_count + left_B_count);
}

/*
	This function is called when client needs to know
	total number of tics the right motor encoder (A and B). 

	\return total 
		total number of right encoder tics read by the tiva
*/
unsigned long int Get_RightEncoder(void)
{
	return (right_A_count + right_B_count);
}

/*
	This function is called when client needs to reset
	the total number of tics read. 
	Not necessary unless it needs to reset to start
	counting up and later compared (like for keeping it
	straight after changing the speed slightly).
*/
void Reset_Encoders(void)
{
	left_A_count = 0;
	left_B_count = 0;
	right_A_count = 0;
	right_B_count = 0;
}

//-------------------------------------------------------------------------------------------------

/*
	This function will be included in the infinite loop function so
	that it will continuously read data and control motors depending
	on the situation (for example, front object getting closer ->
	slow down the motors to avoid collision). Takes in front IR data
	for now since it does not have access to the functions for IR data
	reading.

	still needs to be modified. Can be tested any time.

	\param front_IR 
		a 32 bit data that holds digital data of the distance
		between the IR sensor and the front object.

	\attention
		this portion is not fully completed. It still needs further
		work. So far it can do partial straight movement (speed adjustments)
		and deteft front object to avoid collision.
		
	\todo
		requires further work to complete perfect straight movement
		and turnings at corners
*/
void motor_testing(uint32_t front_IR)
{
		
	long int left_encoder;
	long int right_encoder;
	
		// front IR data 0x000 ~ 0xFFF (further ~ closer)
		//0xDFF ~~ 9cm
		//0xBFF ~~ 11cm
		//0x9FF ~~ 13cm
		//0x7FF ~~ 15cm
		//0x5FF ~~ 21cm
		//0x3F0 ~~ 30cm
		//0x3FF ~~ 32cm
		
		counter++;
		// counter 100 ~~ 1 sec
		
		if(front_IR < 0x3C0)
		{
			if(counter > 100)
			{
//				left_encoder = Get_LeftEncoder();		// gets total number of left encoder tics
//				right_encoder = Get_RightEncoder();	// gets total number of right encoder tics

//				
//				if( ((right_encoder - left_encoder) > 200) )
//				{
//					change_right_speed(50);
//					change_left_speed(50);
//					Reset_Encoders();
//				}
//				else if( ((left_encoder - right_encoder) > 100) )
//				{
//					change_right_speed(53);
//					change_left_speed(45);
//					Reset_Encoders();
//				}
				
				counter = 0;
			}
		}
		else
		{
			if(front_IR > 0xBFF)
			{
				change_right_speed(5);
				change_left_speed(5);
			}
			else if(front_IR > 0x7FF)
			{
				change_right_speed(15);
				change_left_speed(15);
			}
			else if(front_IR > 0x5FF)
			{
				change_right_speed(35);
				change_left_speed(35);
			}
		}
}

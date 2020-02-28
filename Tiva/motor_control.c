#include "motor_control.h"
#include "tm4c123gh6pm.h"

#define FORWARD 1
#define BACKWARD 0

volatile uint32_t left_A_count;
volatile uint32_t left_B_count;
volatile uint32_t right_A_count;
volatile uint32_t right_B_count;

void motor_control_Init(void)
{
		// period 10000 ~= 1.6kHz
		// period 65000 ~= 250Hz
		// period 32500 ~= 500Hz
	uint16_t period = 32500; //freq = clock/period
	uint16_t high = 2;
	int dirright = FORWARD;
	int dirleft = FORWARD;
	
	PWM_left_Init(period, high);
	PWM_right_Init(period, high);
  Dir_left_Init();
  Dir_right_Init();
  //Encoder_left_A_Init();
  //Encoder_left_B_Init();
  //Encoder_right_A_Init();
  //Encoder_right_B_Init();
	//TimerCapture_Init();
	
	int speed = 60;
	
  set_right_speed(speed);
  set_left_speed(speed);
	set_right_dir(dirright);
	set_left_dir(FORWARD);
	
//		if(left_A_count >= 1000)
//		{
//			change_right_speed(10);
//			//change_left_speed(10);
//		}

	for(int i = 0; i < 10000; i++)
		for(int i = 0; i < 1000; i++);
		
	change_right_speed(10);
	change_left_speed(10);
	set_right_dir(BACKWARD);
	set_left_dir(BACKWARD);
	
	for(int i = 0; i < 10000; i++)
		for(int i = 0; i < 1000; i++);
		
	change_right_speed(30);
	change_left_speed(30);
}


// PB6(left) and PB7(right)
// for output PWM motor speed control
//PG650 OF DATASHEET NEEDED FOR MUX TABLE
//pg 737 for timer0 control
void PWM_left_Init(uint16_t period, uint16_t high)
{
	SYSCTL_RCGCTIMER_R |= 0x01;		//activate timer0
	SYSCTL_RCGCGPIO_R |= 0x02;		//activate portB
	while((SYSCTL_PRGPIO_R&0x02)==0){};
	GPIO_PORTB_AMSEL_R = 0x00;		//disable analog function
	GPIO_PORTB_AFSEL_R |= 0x40;		//enable alt func on PB6
	GPIO_PORTB_DEN_R |= 0x40;			//enable digital I/O on PB6
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xF0FFFFFF)+0x07000000;
		//disable digital function of PB6 and activate timer0
	TIMER0_CTL_R &= ~0x00000001;	//disable timer0A during setup
	TIMER0_CFG_R = 0x00000004; 		//configure for 16-bit timer mode
	TIMER0_TAMR_R = 0x0000000A; 	//PWM and periodic mode
	TIMER0_TAILR_R = period-1;		//timer start value
	TIMER0_TAMATCHR_R = period-high-1;	// duty cycle = high/period
	TIMER0_CTL_R |= 0x00000001;		//enable timer0A 16-b, PWM
}
void PWM_right_Init(uint16_t period, uint16_t high)
{
	SYSCTL_RCGCTIMER_R |= 0x01;		//activate timer0
	SYSCTL_RCGCGPIO_R |= 0x02;		//activate portB
	while((SYSCTL_PRGPIO_R&0x02)==0){};
	GPIO_PORTB_AMSEL_R = 0x00;		//disable analog function
	GPIO_PORTB_AFSEL_R |= 0x80;		//enable alt func on PB7
	GPIO_PORTB_DEN_R |= 0x80;			//enable digital I/O on PB7
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0x0FFFFFFF)+0x70000000;
		//disable digital function of PB7 and activate timer0
	TIMER0_CTL_R &= ~0x00000100;	//disable timer0B during setup
	TIMER0_CFG_R = 0x00000004; 		//configure for 16-bit timer mode
	TIMER0_TBMR_R = 0x0000000A; 	//PWM and periodic mode
	TIMER0_TBILR_R = period-1;		//timer start value
	TIMER0_TBMATCHR_R = period-high-1;	// duty cycle = high/period
	TIMER0_CTL_R |= 0x00000100;		//enable timer0B 16-b, PWM
}

// PE4(left) and PE5(right)
// for output GPIO motor direction control
void Dir_left_Init(void)
{
	SYSCTL_RCGC2_R |= 0x10;					// Activate clock for Port E
	unsigned long delay = SYSCTL_RCGC2_R;
	GPIO_PORTE_LOCK_R = 0x4C4F434B;	// unlock GPIO Port E
	GPIO_PORTE_CR_R |= 0x10;					// allow changes to PE4 
	GPIO_PORTE_AMSEL_R = 0x00; 			// disable analog on PE
	GPIO_PORTE_PCTL_R = 0x00;				// PCTL GPIO on PF4
	GPIO_PORTE_DIR_R |= 0x10;				// PE4 out
	GPIO_PORTE_AFSEL_R = 0x00;			// disable alt function PE7-0
	GPIO_PORTE_DEN_R |= 0x10;				// enable digital I/O on PE4
}
void Dir_right_Init(void)
{
	SYSCTL_RCGC2_R |= 0x10;					// Activate clock for Port E
	unsigned long delay = SYSCTL_RCGC2_R;
	GPIO_PORTE_LOCK_R = 0x4C4F434B;	// unlock GPIO Port E
	GPIO_PORTE_CR_R |= 0x20;					// allow changes to PE5
	GPIO_PORTE_AMSEL_R = 0x00; 			// disable analog on PE
	GPIO_PORTE_PCTL_R = 0x00;				// PCTL GPIO on PF5
	GPIO_PORTE_DIR_R |= 0x20;				// PE5 out
	GPIO_PORTE_AFSEL_R = 0x00;			// disable alt function PE7-0
	GPIO_PORTE_DEN_R |= 0x20;				// enable digital I/O on PE5
}




// PB2(leftA) PB3(leftB) PB4(rightA) PB5(rightB)
// for input GPIO encoder data reading
// PB2 (leftA) Timer3A
// PB3 (leftB) Timer3B
// PB4 (rightA) Timer1A
// PB5 (rightB) Timer1B
void Encoder_left_A_Init(void)
{
	//DisableInterrupts();
	//__DisableInterrupts;

	SYSCTL_RCGCTIMER_R |= 0x08; 	// activate timer3
	SYSCTL_RCGCGPIO_R |= 0x02;		// activate portB
	GPIO_PORTB_DEN_R |= 0x04;			// enable digital I/O on PB2
	GPIO_PORTB_AFSEL_R |= 0x04; 	// enable alt function on PB2
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFFF0FF) + 0x00000700;
			//disable digital function of PB2 and activate timer3
	TIMER3_CTL_R &= ~0x00000001;	// disable Timer3A during setup
	TIMER3_CFG_R = 0X00000004;		// configer for 16-bit timer mode
	TIMER3_TAMR_R = 0x00000007; 	// configure for input capture mode
	TIMER3_CTL_R &= ~(0x000C);		// TAEVENT is rising edge
	TIMER3_TAILR_R = 0x0000FFFF;	// start value
	TIMER3_IMR_R |= 0x00000004;		// enable capture match interrupt
	TIMER3_ICR_R = 0x00000004;		// clear Timer3A capture flag
	TIMER3_CTL_R |= 0x00000001; 	// enable Timer3A
	NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF)|0x40000000; 
			// Timer3A = priority 2
	NVIC_EN0_R = 0x00080000;			// enable interrupt 19 in NVIC
	left_A_count = 0;
	//EnableInterrupts();
}

void Encoder_left_B_Init(void)
{
	SYSCTL_RCGCTIMER_R |= 0x08; 	// activate timer3
	SYSCTL_RCGCGPIO_R |= 0x02;		// activate portB
	GPIO_PORTB_DEN_R |= 0x08;			// enable digital I/O on PB3
	GPIO_PORTB_AFSEL_R |= 0x08; 	// enable alt function on PB3
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFF0FFF) + 0x00007000;
			//disable digital function of PB3 and activate timer3
	TIMER3_CTL_R &= ~0x00000100;	// disable Timer3B during setup
	TIMER3_CFG_R = 0X00000004;		// configer for 16-bit timer mode
	TIMER3_TBMR_R = 0x00000007; 	// configure for input capture mode
	TIMER3_CTL_R &= ~(0x000C);		// TAEVENT is rising edge
	TIMER3_TBILR_R = 0x0000FFFF;	// start value
	TIMER3_IMR_R |= 0x00000400;		// enable capture match interrupt
	TIMER3_ICR_R = 0x00000004;		// clear Timer3A capture flag
	TIMER3_CTL_R |= 0x00000100; 	// enable Timer3A
	NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF)|0x40000000; 
			// Timer3B = priority 2
	NVIC_EN0_R = 0x00080000;			// enable interrupt 19 in NVIC
	
	left_B_count = 0;
}

void Encoder_right_A_Init(void)
{
	SYSCTL_RCGCTIMER_R |= 0x02; 	// activate timer1
	SYSCTL_RCGCGPIO_R |= 0x02;		// activate portB
	GPIO_PORTB_DEN_R |= 0x10;			// enable digital I/O on PB4
	GPIO_PORTB_AFSEL_R |= 0x10; 	// enable alt function on PB4
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFF0FFFF) + 0x00070000;
			//disable digital function of PB4 and activate timer1
	TIMER1_CTL_R &= ~0x00000001;	// disable Timer1A during setup
	TIMER1_CFG_R = 0X00000004;		// configer for 16-bit timer mode
	TIMER1_TAMR_R = 0x00000007; 	// configure for input capture mode
	TIMER1_CTL_R &= ~(0x000C);		// TAEVENT is rising edge
	TIMER1_TAILR_R = 0x0000FFFF;	// start value
	TIMER1_IMR_R |= 0x00000004;		// enable capture match interrupt
	TIMER1_ICR_R = 0x00000004;		// clear Timer1A capture flag
	TIMER1_CTL_R |= 0x00000001; 	// enable Timer1A
	NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF)|0x40000000; 
			// Timer1A = priority 2
	NVIC_EN0_R = 0x00080000;			// enable interrupt 19 in NVIC
	
	right_A_count = 0;
}

void Encoder_right_B_Init(void)
{
	SYSCTL_RCGCTIMER_R |= 0x02; 	// activate timer1
	SYSCTL_RCGCGPIO_R |= 0x02;		// activate portB
	GPIO_PORTB_DEN_R |= 0x20;			// enable digital I/O on PB5
	GPIO_PORTB_AFSEL_R |= 0x20; 	// enable alt function on PB5
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFF0FFFFF) + 0x00700000;
			//disable digital function of PB5 and activate timer1
	TIMER1_CTL_R &= ~0x00000100;	// disable Timer1B during setup
	TIMER1_CFG_R = 0X00000004;		// configer for 16-bit timer mode
	TIMER1_TAMR_R = 0x00000007; 	// configure for input capture mode
	TIMER1_CTL_R &= ~(0x000C);		// TAEVENT is rising edge
	TIMER1_TAILR_R = 0x0000FFFF;	// start value
	TIMER1_IMR_R |= 0x00000004;		// enable capture match interrupt
	TIMER1_ICR_R = 0x00000004;		// clear Timer1B capture flag
	TIMER1_CTL_R |= 0x00000100; 	// enable Timer1B
	NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF)|0x40000000; 
			// Timer1B = priority 2
	NVIC_EN0_R = 0x00080000;			// enable interrupt 19 in NVIC
	
	right_B_count = 0;
}

void Timer3A_Handler(void)
{
	TIMER3_ICR_R = 0x00000004;
	//left_A_count++;
	change_left_speed(10);
}
void Timer3B_Handler(void)
{
	TIMER3_ICR_R = 0x00000004;
	left_B_count++;
}
void Timer1A_Handler(void)
{
	TIMER1_ICR_R = 0x00000004;
	right_A_count++;
}
void Timer1B_Handler(void)
{
	TIMER1_ICR_R = 0x00000004;
	right_B_count++;
}


void TimerCapture_Init(void)
{
	DisableInterrupts();
	SYSCTL_RCGCTIMER_R |= 0x01;
	SYSCTL_RCGCGPIO_R |= 0x02;
	left_A_count = 0;
	GPIO_PORTB_DEN_R |= 0x40;
	GPIO_PORTB_AFSEL_R |= 0x40;
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xF0FFFFFF)+0x07000000;
	TIMER0_CTL_R &= ~0x01;
	TIMER0_CFG_R = 0x04;
	TIMER0_TAMR_R = 0x07;
	TIMER0_CTL_R &= ~(0x000C);
	TIMER0_TAILR_R = 0xFFFF;
	TIMER0_IMR_R |= 0x04;
	TIMER0_ICR_R = 0x04;
	TIMER0_CTL_R |= 0x01;
	NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF)|0x40000000;
	NVIC_EN0_R = 0x00080000;
	EnableInterrupts();
}
void Timer0A_Handler(void)
{
	TIMER0_ICR_R = 0x04;
	left_A_count = left_A_count + 1;
	change_right_speed(10);
}




void set_left_speed(int speed)
{
	uint16_t high = speed * 325;
	
	TIMER0_TAMATCHR_R = TIMER0_TAILR_R-high;
}
void set_right_speed(int speed)
{
	uint16_t high = speed * 325;
	
	TIMER0_TBMATCHR_R = TIMER0_TBILR_R-high;
}

void set_right_dir(int direction)
{
	if(direction == BACKWARD)
		GPIO_PORTE_DATA_R |= 0x20;
	else
		GPIO_PORTE_DATA_R &= 0xDF;
}
void set_left_dir(int direction)
{
	if(direction == FORWARD)
		GPIO_PORTE_DATA_R |= 0x10;
	else
		GPIO_PORTE_DATA_R &= 0xEF;
}

void change_left_speed(int speed)
{
	uint16_t high = speed * 325;
	
	uint16_t goal = TIMER0_TBILR_R - high;
	
	if(TIMER0_TAMATCHR_R < goal)
	{
		for(int i = 0; i < goal - TIMER0_TAMATCHR_R; i++)
		{
			TIMER0_TAMATCHR_R += 1;
			for(int i = 0; i < 200; i++);
		}
	}
	else	
	{
		for(int i = 0; i < TIMER0_TAMATCHR_R - goal; i++)
		{
			TIMER0_TAMATCHR_R -= 1;
			for(int i = 0; i < 200; i++);
		}
	}
			
}
void change_right_speed(int speed)
{
	uint16_t high = speed * 325;
	
	uint16_t goal = TIMER0_TAILR_R - high;
	
	if(TIMER0_TBMATCHR_R < goal)
	{
		for(int i = 0; i < goal - TIMER0_TBMATCHR_R; i++)
		{
			TIMER0_TBMATCHR_R += 1;
			for(int i = 0; i < 200; i++);
		}
	}
	else	
	{
		for(int i = 0; i < TIMER0_TBMATCHR_R - goal; i++)
		{
			TIMER0_TBMATCHR_R -= 1;
			for(int i = 0; i < 200; i++);
		}
	}
}

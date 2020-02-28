//void portB_Init(void);
//void portE_Init(void);
#include <stdint.h>


void motor_control_Init(void);

void PWM_left_Init(uint16_t period, uint16_t high);
void PWM_right_Init(uint16_t period, uint16_t high);
void Dir_left_Init(void);
void Dir_right_Init(void);
void Encoder_left_A_Init(void);
void Encoder_left_B_Init(void);
void Encoder_right_A_Init(void);
void Encoder_right_B_Init(void);
void TimerCapture_Init(void);

void set_right_speed(int speed);
void set_left_speed(int speed);
void set_right_dir(int direction);
void set_left_dir(int direction);

void change_left_speed(int speed);
void change_right_speed(int speed);

#ifndef MOTOR_H
#define MOTOR_H

#include "LPC407x_8x_177x_8x.h"

#include "GPIO.h"
#include "PWM.h"


#define	MOTOR1_IN1_IOCON_ADDRESS	0x4002C280 //p39 - p5.0
#define	MOTOR1_IN2_IOCON_ADDRESS	0x4002C284 //p38 - p5.1
#define	MOTOR2_IN1_IOCON_ADDRESS	0x4002C288 //p32 - p5.2
#define	MOTOR2_IN2_IOCON_ADDRESS	0x4002C28C //p31 - p5.3

#define MOTOR1_IN1_MASK (1<<0)
#define MOTOR1_IN2_MASK (1<<1)
#define MOTOR2_IN1_MASK (1<<2)
#define MOTOR2_IN2_MASK (1<<3)

#define MOTOR1_SPEED_IOCON_ADDRESS	0x4002C0DC // P6 - P1.23
#define MOTOR2_SPEED_IOCON_ADDRESS	0x4002C0D0 // P7 - P1.20

#define MOTOR_PWM_INDEX 1

#define MOTOR_DIR_FORWARD 0x2
#define MOTOR_DIR_BACKWARD 0x1
#define MOTOR_DIR_BRAKE 0x0

#define TURN_DIR_LEFT 1
#define TURN_DIR_RIGHT -1

#define LEFT_MOTOR_INDEX 0
#define RIGHT_MOTOR_INDEX 1

#define IOCON_SPEED_SENSOR_ADDRESS	0x4002C010
#define IOCON_SPEED_SENSOR	*((volatile uint32_t*)(IOCON_SPEED_SENSOR_ADDRESS)) //P34 - P0.4

void Init_Motor(uint8_t);
void Init_Motor_PWM();
void Set_Motor_Speed(uint8_t motorIndex, int8_t speed);
void Set_Motor_Direction(uint8_t motorIndex, uint8_t dir);
void Resume_Motor(uint8_t motorIndex);
void Pause_Motor(uint8_t motorIndex);
volatile uint32_t getTickCount();
void Turn(int8_t dir);


extern volatile uint32_t tickCount,
									       rotateUntilTick;

#endif
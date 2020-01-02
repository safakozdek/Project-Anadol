#include "Motor.h"
#include "stdlib.h"

volatile uint32_t tickCount = 0,
									rotateUntilTick = 0;

const uint32_t MOTORS_IN_MASKS[][2] = {{MOTOR1_IN1_MASK, MOTOR1_IN2_MASK}, {MOTOR2_IN1_MASK, MOTOR2_IN2_MASK}};
const uint32_t MOTORS_IOCON_ADDRS[][3] = {{MOTOR1_IN1_IOCON_ADDRESS, MOTOR1_IN2_IOCON_ADDRESS, MOTOR1_SPEED_IOCON_ADDRESS},
																					{MOTOR2_IN1_IOCON_ADDRESS, MOTOR2_IN2_IOCON_ADDRESS, MOTOR2_SPEED_IOCON_ADDRESS}};
const uint32_t MOTORS_IOCON_VALS[][3] = {{0x0, 0x0, 0x2},
																					{0x0, 0x0, 0x2}};
const uint32_t MOTORS_GPIO_MASKS[][2] = {{MOTOR1_IN1_MASK, MOTOR1_IN2_MASK}, {MOTOR2_IN1_MASK, MOTOR2_IN2_MASK}};
GPIO_TypeDef* const MOTORS_GPIO_PORTS[][2] = {{PORT5, PORT5}, {PORT5, PORT5}};
const uint8_t MOTOR_PWM_CHANNELS[] = {4, 2};

uint8_t motorSpeed[] = {0}, motorDirection[] = {MOTOR_DIR_BRAKE, MOTOR_DIR_BRAKE};

void IOCON_Func_Set(uint32_t ioconAddr, uint8_t func) {
	*((volatile uint32_t*)(ioconAddr)) = (*((volatile uint32_t*)(ioconAddr)) & ~(0x7)) | func;
}

void Init_Tick_Interrupt(void);

void Init_Motor(uint8_t motorIndex) {
	uint32_t i;
	
	// Set IOCON functions
	for (i = 0; i < 3; ++i) {
		IOCON_Func_Set(MOTORS_IOCON_ADDRS[motorIndex][i], MOTORS_IOCON_VALS[motorIndex][i]);
	}
	
	// Set IN1 and IN2 as output
	GPIO_DIR_Write(MOTORS_GPIO_PORTS[motorIndex][0], MOTORS_GPIO_MASKS[motorIndex][0], 1);
	GPIO_DIR_Write(MOTORS_GPIO_PORTS[motorIndex][1], MOTORS_GPIO_MASKS[motorIndex][1], 1);
	
	// Initially break the motors
	GPIO_PIN_Write(MOTORS_GPIO_PORTS[motorIndex][0], MOTORS_GPIO_MASKS[motorIndex][0], 0);
	GPIO_PIN_Write(MOTORS_GPIO_PORTS[motorIndex][1], MOTORS_GPIO_MASKS[motorIndex][1], 0);
	
	Init_Tick_Interrupt();
}

void Init_Motor_PWM() {
	PWM_Init(MOTOR_PWM_INDEX, MOTOR_PWM_CHANNELS, 2);
	PWM_Cycle_Rate(MOTOR_PWM_INDEX, 20);
	
	Set_Motor_Speed(LEFT_MOTOR_INDEX, 0);
	Set_Motor_Speed(RIGHT_MOTOR_INDEX, 0);
}

// speed: 0 to 100
void Set_Motor_Speed(uint8_t motorIndex, int32_t speed) {
	if (speed > 0)
		Set_Motor_Direction(motorIndex, MOTOR_DIR_FORWARD);
	else
		Set_Motor_Direction(motorIndex, MOTOR_DIR_BACKWARD);
	
	speed = abs(speed);
	
	if (speed > 90)
		speed = 90;
	
	PWM_Write(MOTOR_PWM_INDEX, MOTOR_PWM_CHANNELS[motorIndex], speed);
}

void Set_Motor_Direction(uint8_t motorIndex, uint8_t dir) {
	//motorDirection[motorIndex] = dir;
	GPIO_PIN_Write(MOTORS_GPIO_PORTS[motorIndex][0], MOTORS_GPIO_MASKS[motorIndex][0], dir & 1);
	GPIO_PIN_Write(MOTORS_GPIO_PORTS[motorIndex][1], MOTORS_GPIO_MASKS[motorIndex][1], (dir >> 1) & 1);
}

void Resume_Motor(uint8_t motorIndex) {
	//Set_Motor_Direction(motorIndex, motorDirection[motorIndex]);
	//Set_Motor_Speed(motorIndex, motorSpeed[motorIndex]);
}

void Pause_Motor(uint8_t motorIndex) {
//	PWM_Write(MOTOR_PWM_INDEX, MOTOR_PWM_CHANNELS[motorIndex], 100); 
	//GPIO_PIN_Write(MOTORS_GPIO_PORTS[motorIndex][0], MOTORS_GPIO_MASKS[motorIndex][0], MOTOR_DIR_BRAKE & 1);
//	GPIO_PIN_Write(MOTORS_GPIO_PORTS[motorIndex][1], MOTORS_GPIO_MASKS[motorIndex][1], (MOTOR_DIR_BRAKE >> 1) & 1);
}

void Turn(int8_t dir, uint8_t power) { // 1: right, -1: left
	rotateUntilTick = getTickCount() + TICKS_FOR_TURN;
	Set_Motor_Speed(LEFT_MOTOR_INDEX, power * dir);
	Set_Motor_Speed(RIGHT_MOTOR_INDEX, -power * dir);
}


void Init_Tick_Interrupt() {
	//Change the functionality of the interrupt sensor's pin as T2_CAP_0
	IOCON_SPEED_SENSOR = (IOCON_SPEED_SENSOR & ~(0x7)) | 0x3;
}

uint32_t getTickCount(){
	return tickCount;
}


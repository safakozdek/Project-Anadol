#include "LPC407x_8x_177x_8x.h"

#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 

#include "Library/Serial.h"
#include "Library/Ultrasonic.h"
#include "Library/HM10.h"
#include "Library/Timer.h"
#include "Library/ADC.h"
#include "Library/GPIO.h"
#include "Library/Motor.h"
#include "Library/LEDs.h"


#define UART_READ_BUFFER_SIZE 512
#define UART_WRITE_BUFFER_SIZE 512

char uartReadBuffer[UART_READ_BUFFER_SIZE] = "";
char uartWriteBuffer[UART_WRITE_BUFFER_SIZE] = ""; 




void init() {	
	Ultrasonic_Init();
	Ultrasonic_Trigger_Timer_Init();
	Ultrasonic_Capture_Timer_Init();
	
	Serial_Init();
	HM10_Init();
	
	ADC_Init();
	ADC_Start();
	
	//Timer_Init();
	
	Init_Motor(0);
	Init_Motor_PWM();
	
	Init_LED();
	
	Ultrasonic_Start_Trigger_Timer();
}

char readWhenAvailable() {
	while (!serialNewDataAvailable);
	
	serialNewDataAvailable = 0;
	
	return serialReceivedCharacter;
}

void sendStatus() {
	char distStrBuf[512] = "";
	
	sprintf(distStrBuf, "{\"front_dist\": %f, \"back_dist\": %f, \"light_left\": %d, \"light_right\": %d, \"pot\": %d }\r\n",
				  ultrasonicSensorsDurations[0] / 58.0,
					ultrasonicSensorsDurations[1] / 58.0,
				  ADC_GetLastValue(LDR1_CHANNEL_INDEX),
					ADC_GetLastValue(LDR2_CHANNEL_INDEX),
					ADC_GetLastValue(POTENTIOMETER_CHANNEL_INDEX));
	
	HM10_SendCommand(distStrBuf);
}

void update() {
	while (!HM10NewDataAvailable);
	
	HM10_SendCommand(NextCommand);
	
	if (strcmp(NextCommand, "STATUS\r\n") == 0) {
		sendStatus();
	} else if (strcmp(NextCommand, "FORWARD\r\n")  == 0){
		//Set_Motor_Direction(0, MOTOR_DIR_FORWARD);
		Set_Motor_Speed(0, 90);
		frontLED();
	} else if (strcmp(NextCommand, "BACK\r\n")  == 0){
		// Set_Motor_Direction(0, MOTOR_DIR_BACKWARD);
		Set_Motor_Speed(0, -90);
		backLED();
	} else if (strcmp(NextCommand, "STOP\r\n")  == 0){
		Set_Motor_Speed(0, 0);
		turnOffLED();
	} else if (strcmp(NextCommand, "LEFT\r\n")  == 0){
		leftLED();
		Turn(TURN_DIR_LEFT);
	} else if (strcmp(NextCommand, "RIGHT\r\n")  == 0){
		rightLED();
	} 
		
	memset(NextCommand, 0, HM10BufferSize);
	HM10NewDataAvailable = 0;
}	

int main() {
	init();
	
	while(1) {
		update();
	}
}


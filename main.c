#include "LPC407x_8x_177x_8x.h"

#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <math.h> 

#include "Library/Serial.h"
#include "Library/Ultrasonic.h"
#include "Library/HM10.h"
#include "Library/Timer.h"
#include "Library/ADC.h"
#include "Library/GPIO.h"
#include "Library/Motor.h"
#include "Library/LEDs.h"
#include "Library/wait.h"

#define UART_READ_BUFFER_SIZE 512
#define UART_WRITE_BUFFER_SIZE 512

#define kP 8
#define kD 0
#define finishLight 1750

char currentMode[15] = "TEST"; // TEST, AUTONOMUS
char currentState[15] = "IDLE"; // IDLE, FORWARD, BACK, RIGHT, LEFT
double lastError = 0;
uint8_t isTurning = 0;
float leftPower=0;
float rightPower=0;

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
	
	Init_Motor(LEFT_MOTOR_INDEX);
	Init_Motor(RIGHT_MOTOR_INDEX);
	Init_Motor_PWM();
	
	Init_LED();
	
	Ultrasonic_Start_Trigger_Timer();
}

char readWhenAvailable() {
	while (!serialNewDataAvailable);
	
	serialNewDataAvailable = 0;
	
	return serialReceivedCharacter;
}

uint32_t getRightLDR(){
	return ADC_MAX_VALUE - ADC_GetLastValue(RIGHT_LDR_CHANNEL_INDEX);
}

uint32_t getLeftLDR(){
	return ADC_MAX_VALUE - ADC_GetLastValue(LEFT_LDR_CHANNEL_INDEX);
}

float getPotentiometer(){
	return ADC_GetLastValue(POTENTIOMETER_CHANNEL_INDEX) / 4096.0;
}

void sendStatus() {
	char distStrBuf[512] = "";
	
	sprintf(distStrBuf, "{\"front_dist\": %f, \"back_dist\": %f, \"light_left\": %d, \"light_right\": %d, \"pot\": %f }\r\n",
				  ultrasonicSensorsDurations[0] / 58.0,
					ultrasonicSensorsDurations[1] / 58.0,
				  getLeftLDR(),
					getRightLDR(),
					getPotentiometer());
	
	HM10_SendCommand(distStrBuf);
}

void sendAutonomousStatus(float leftPower, float rightPower) {
	char distStrBuf[512] = "";
	
	sprintf(distStrBuf, "{\"left_power\": %f, \"right_power\": %f }\r\n",
				  leftPower,
					rightPower);
	
	HM10_SendCommand(distStrBuf);
}

float getBackUltrasonic(){
	return (ultrasonicSensorsDurations[BACK_ULTRASONIC_INDEX] / 58.0);
}

float getFrontUltrasonic(){
	return (ultrasonicSensorsDurations[FRONT_ULTRASONIC_INDEX] / 58.0);
}
 
void moveAutonomous(float baseSpeed){
    uint32_t leftLDRValue = getLeftLDR();
    uint32_t rightLDRValue = getRightLDR();
		uint32_t minLightVal = leftLDRValue < rightLDRValue ? leftLDRValue: rightLDRValue;
    
		float backSonar = getBackUltrasonic();
    float frontSonar = getFrontUltrasonic();
	
	
    double error = frontSonar - backSonar;
    double correction = (error * kP) + ((error - lastError) * kD);

	  if (frontSonar > 100){
			rightPower = baseSpeed;
			leftPower = 0;
		} else if(backSonar > 100){
			return;
		} /*else if(backSonar < 100 && backSonar > 30){
			return;
		} else if(backSonar > 100){
			return;
		}*/ else if (frontSonar < backSonar) {
			rightPower = (baseSpeed + (correction * 3)) < 0 ? 0 : (baseSpeed + (correction * 3));
			leftPower = baseSpeed - correction;
		} else if (fabs(frontSonar - backSonar) < 1) {
			rightPower = baseSpeed;
			leftPower = baseSpeed ;
		} else {
			rightPower = baseSpeed + correction * 1.5;
			leftPower = (baseSpeed - (correction * 7)) < -5 ? -5 : (baseSpeed - (correction * 7));
		}
	 
		if(minLightVal > finishLight){
        HM10_SendCommand("FINISH");
        strcpy(currentState, "IDLE");
        return;
    }

		Set_Motor_Speed(LEFT_MOTOR_INDEX, leftPower);
		Set_Motor_Speed(RIGHT_MOTOR_INDEX, rightPower);
    lastError = error;
}

void update() {
	__WFI();
	
	if(HM10NewDataAvailable){
		HM10_SendCommand(NextCommand);
		
		if (strcmp(NextCommand, "STATUS\r\n") == 0) {
				sendStatus();
			
				if (strcmp(currentMode, "AUTO") == 0)
					sendAutonomousStatus(leftPower, rightPower);
		} else if (strcmp(NextCommand, "FORWARD\r\n")  == 0){
				strcpy(currentState, "FORWARD");
		} else if (strcmp(NextCommand, "BACK\r\n")  == 0){
				strcpy(currentState, "BACK");
		} else if (strcmp(NextCommand, "STOP\r\n")  == 0){
				strcpy(currentState, "IDLE");
		} else if (strcmp(NextCommand, "LEFT\r\n")  == 0){
				strcpy(currentState, "LEFT");
		} else if (strcmp(NextCommand, "RIGHT\r\n")  == 0){
				strcpy(currentState, "RIGHT");
		}else if (strcmp(NextCommand, "TEST\r\n")  == 0){
				HM10_SendCommand("TESTING\r\n");
				strcpy(currentMode, "TEST");
				strcpy(currentState, "IDLE");
		}else if (strcmp(NextCommand, "AUTO\r\n")  == 0){
				HM10_SendCommand("AUTONOMOUS\r\n");
        strcpy(currentMode, "AUTO");
        strcpy(currentState, "IDLE");
		}else if (strcmp(NextCommand, "START\r\n")  == 0){
        strcpy(currentState, "START");
		}
		
		memset(NextCommand, 0, HM10BufferSize);
		HM10NewDataAvailable = 0;
	}
	
	if(strcmp(currentMode, "TEST") == 0){
		uint32_t leftLDRValue = getLeftLDR();
    uint32_t rightLDRValue = getRightLDR();
		
		float potentVal = getPotentiometer();
		if (potentVal < 0.1) potentVal = 0;
		else if (potentVal > 0.9) potentVal = 1;
		
		if((leftLDRValue + rightLDRValue) / 2 > finishLight){
			Set_Motor_Speed(LEFT_MOTOR_INDEX, 0);
			Set_Motor_Speed(RIGHT_MOTOR_INDEX, 0);
			
			turnOffLED();
		} else if (strcmp(currentState, "LEFT") == 0) {
			if(!isTurning){
				Turn(TURN_DIR_LEFT, (int)(80 * potentVal));
				leftLED();
				isTurning = 1;
			}
			if(isTurnComplete){
				isTurning = 0;
				isTurnComplete = 0;
				strcpy(currentState, "IDLE");
			}
    } else if(strcmp(currentState, "RIGHT") == 0){
			if(!isTurning){
				Turn(TURN_DIR_RIGHT, (int)(80 * potentVal));
				rightLED();
				isTurning = 1;
			}
			if(isTurnComplete){
				isTurning = 0;
				isTurnComplete = 0;
				strcpy(currentState, "IDLE");
			}
		} else if(strcmp(currentState, "FORWARD") == 0){
      Set_Motor_Speed(0, (int)(90 * potentVal));
			Set_Motor_Speed(1, (int)(90 * potentVal));
			
			frontLED();
		} else if(strcmp(currentState, "BACK") == 0){
			Set_Motor_Speed(0, (int)(-90 * potentVal));
			Set_Motor_Speed(1, (int)(-90 * potentVal));
			
			backLED();
		} else if(strcmp(currentState, "IDLE") == 0){
      Set_Motor_Speed(0, 0);
			Set_Motor_Speed(1, 0);
			
			turnOffLED();
		}
	} else if(strcmp(currentMode, "AUTO") == 0){
			if(strcmp(currentState, "START") == 0){
					frontLED();
					moveAutonomous(85);
			} else if(strcmp(currentState, "IDLE") == 0){
					turnOffLED();
				
					Set_Motor_Speed(LEFT_MOTOR_INDEX, 0);
					Set_Motor_Speed(RIGHT_MOTOR_INDEX, 0);
      }
  }
}

int main() {
	init();

	while(1) {
		update();
	}
}


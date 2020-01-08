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

#define kP 6
#define kD 1.5
#define finishLight 1550

char currentMode[15] = "TEST"; // TEST, AUTONOMUS
char currentState[15] = "IDLE"; // IDLE, FORWARD, BACK, RIGHT, LEFT
uint8_t isTurning = 0;

float leftPower=0;
float rightPower=0;

uint8_t closeCount = 0;
uint8_t farCount = 0;

double lastError = 0;
double lastFront = 0;
double lastBack = 0;

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
	
	sprintf(distStrBuf, "{\"front_distance\": %d, \"back_distance\": %d, \"light_level_left\": %d, \"light_level_right\": %d, \"op_mode\": \"%s\" }\r\n",
				  (int)(ultrasonicSensorsDurations[0] / 58.0),
					(int)(ultrasonicSensorsDurations[1] / 58.0),
				  getLeftLDR(),
					getRightLDR(),
					strcmp(currentMode, "TEST") == 0 ? "TEST" : "AUTO");
	
	HM10_SendCommand(distStrBuf);
}

void sendAutonomousStatus(float leftPower, float rightPower) {
	return;
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

	  if (frontSonar > 80 || backSonar > 80 ){
			if(frontSonar - lastFront > 0 || backSonar - lastBack > 0){
				farCount++;
				if(farCount > 5){
					rightPower = baseSpeed/2;
					leftPower = 0;
				}
			} else if(frontSonar - lastFront < 0 || backSonar - lastBack < 0){
				closeCount++;
				if(closeCount > 5){
					leftPower = baseSpeed/2;
					rightPower = 0;
				}
			}
		} else{
			farCount = 0;
			closeCount = 0;
			
				if(backSonar > 31 && frontSonar > 31){
					if(frontSonar < backSonar){
						rightPower = baseSpeed / 3;
						leftPower = baseSpeed /2;
					} else {
						rightPower = 3 * baseSpeed / 4;
						leftPower = 0;
					}
				} else if(backSonar < 16 || frontSonar < 16){
					if(frontSonar <= backSonar){
						rightPower = 0;
						leftPower = baseSpeed;
					} else {
						rightPower = 3 * baseSpeed /4;
						leftPower = baseSpeed / 3;
					}
				} else if (frontSonar < backSonar && fabs(frontSonar - backSonar) > 0.75) {
					rightPower = (baseSpeed + (correction * 4)) < 0 ? 0 : (baseSpeed + (correction * 4));
					leftPower = baseSpeed - correction;
				} else if (fabs(frontSonar - backSonar) < 1.25) {
					rightPower = baseSpeed;
					leftPower = baseSpeed;
				} else if(backSonar < frontSonar){
					rightPower = baseSpeed + correction;
					leftPower = (baseSpeed - (correction * 5)) < -2 ? -2 : (baseSpeed - (correction * 5));
				}
		}
	 
		if(minLightVal > finishLight){
        HM10_SendCommand("FINISH\r\n");
        strcpy(currentState, "IDLE");
        return;
    }

		Set_Motor_Speed(LEFT_MOTOR_INDEX, leftPower);
		Set_Motor_Speed(RIGHT_MOTOR_INDEX, rightPower);
    lastError = error;
		lastFront = frontSonar;
		lastBack = backSonar;
		wait(60);
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
				isTurning = 0;
				isTurnComplete = 0;
		} else if (strcmp(NextCommand, "LEFT\r\n")  == 0){
				strcpy(currentState, "LEFT");
				leftLED();
		} else if (strcmp(NextCommand, "RIGHT\r\n")  == 0){
				strcpy(currentState, "RIGHT");
				rightLED();
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
				lastError = 0;
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
				Turn(TURN_DIR_LEFT, (int)(80));
				isTurning = 1;
			}
			if(isTurnComplete){
				isTurning = 0;
				isTurnComplete = 0;
				strcpy(currentState, "IDLE");
			}
    } else if(strcmp(currentState, "RIGHT") == 0){
			if(!isTurning){
				Turn(TURN_DIR_RIGHT, (int)(80));
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
					moveAutonomous(80);
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


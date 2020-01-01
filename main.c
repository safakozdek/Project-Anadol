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

#define kP 2.5
#define kD 0.1
#define startLightDetection 2500
#define finishLight 1600

char currentMode[15] = "TEST";
char currentState[15] = "IDLE";
double lastError = 0;
uint8_t isTurning = 0;

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

uint32_t getRightLDR(){
	return ADC_GetLastValue(2);
}

uint32_t getLeftLDR(){
	return ADC_GetLastValue(3);
}

float getPotentiometer(){
	int x = ADC_GetLastValue(4);
	return ADC_GetLastValue(4) / 4096.0;
}

float getBackUltrasonic(){
	return (ultrasonicSensorsDurations[1] / 58.0);
}

float getFrontUltrasonic(){
	return (ultrasonicSensorsDurations[0] / 58.0);
}

void moveAutonomous(float baseSpeed){
    uint32_t leftLDRValue = getLeftLDR();
    uint32_t rightLDRValue = getRightLDR();
		uint32_t minLightVal = leftLDRValue < rightLDRValue ? leftLDRValue: rightLDRValue;
    
		float backSonar = getBackUltrasonic();
    float frontSonar = getFrontUltrasonic();
    float leftPower = baseSpeed;
    float rightPower = baseSpeed;

    double error = frontSonar - backSonar;
    double correction = (error * kP) + ((error - lastError) * kD);

    if(backSonar < 19 || frontSonar < 19 ){
        leftPower = baseSpeed - 7;
        rightPower = baseSpeed + 7;
    } else if (backSonar > 31 || frontSonar > 31){
        leftPower = baseSpeed + 7;
        rightPower = baseSpeed - 7;
    } else {
        leftPower = baseSpeed + correction;
        rightPower = baseSpeed - correction;
    }

    //Closing to the light
		if(minLightVal > 300 && minLightVal < finishLight){
        HM10_SendCommand("FINISH");
        strcpy(currentState, "IDLE");
        return;
    } else if(minLightVal > 300 && minLightVal < startLightDetection){
        leftPower = (leftPower * ((minLightVal - finishLight) / (startLightDetection-finishLight))) + 5;
        rightPower = (rightPower * ((minLightVal - finishLight) / (startLightDetection-finishLight))) + 5;
    } 

		Set_Motor_Speed(0, leftPower);
		Set_Motor_Speed(1, rightPower);
    lastError = error;
}

void update() {
	if(HM10NewDataAvailable){
		HM10_SendCommand(NextCommand);
		if (strcmp(NextCommand, "STATUS\r\n") == 0) {
				sendStatus();
		} else if (strcmp(NextCommand, "FORWARD\r\n")  == 0){
				strcpy(currentState, "FORWARD");
		} else if (strcmp(NextCommand, "BACK\r\n")  == 0){
				strcpy(currentState, "BACK");
		} else if (strcmp(NextCommand, "STOP\r\n")  == 0){
				strcpy(currentState, "IDLE");
		} else if (strcmp(NextCommand, "LEFT\r\n")  == 0){
				strcpy(currentState, "LEFT");
		} else if (strcmp(NextCommand, "RIGHT\r\n")  == 0){
				strcpy(currentState, "BACK");
		}else if (strcmp(NextCommand, "TEST\r\n")  == 0){
				HM10_SendCommand("TESTING\r\n");
				strcpy(currentMode, "TEST");
				strcpy(currentState, "IDLE");
		}else if (strcmp(NextCommand, "AUTO\r\n")  == 0){
				HM10_SendCommand("AUTONOMOUS\r\n");
        strcpy(currentMode, "AUTO");
        strcpy(currentState, "IDLE");
		}
	}
	
	if(strcmp(currentMode, "TEST") == 0){
		uint32_t leftLDRValue = getLeftLDR();
    uint32_t rightLDRValue = getRightLDR();
		
		float potentVal = getPotentiometer();
		if(potentVal < 0.1) potentVal = 0;
		else if(potentVal > 0.9) potentVal = 1;
		
		if((leftLDRValue + rightLDRValue) / 2 < finishLight){
			Set_Motor_Speed(0, 0);
			Set_Motor_Speed(1, 0);
			turnOffLED();
			
		} else if(strcmp(currentState, "LEFT") == 0){
			if(!isTurning){
				Turn(TURN_DIR_LEFT, (int)(80 * potentVal));
				rightLED();
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
					moveAutonomous(80);
			} else if(strcmp(currentState, "IDLE") == 0){
					turnOffLED();
					Set_Motor_Speed(0, 0);
					Set_Motor_Speed(1, 0);
      }
  }
	
	memset(NextCommand, 0, HM10BufferSize);
	HM10NewDataAvailable = 0;
	wait(50);
}

int main() {
	init();
	
	while(1) {
		update();
	}
}


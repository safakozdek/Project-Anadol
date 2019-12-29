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
	
	Timer_Init();
	
	ADC_Start();
	
	Ultrasonic_Start_Trigger_Timer();
}

char readWhenAvailable() {
	while (!serialNewDataAvailable);
	
	serialNewDataAvailable = 0;
	
	return serialReceivedCharacter;
}

void sendStatus() {
	char distStrBuf[512] = "";
	
	sprintf(distStrBuf, "{\"front_dist\": %f, \"back_dist\": %f, \"light\": %d}\r\n",
				  ultrasonicSensorsDurations[0] / 58.0,
					ultrasonicSensorsDurations[1] / 58.0,
				  ADC_GetLastValue());
	
	HM10_SendCommand(distStrBuf);
}

void update() {
	while (!HM10NewDataAvailable);
	
	HM10_SendCommand(NextCommand);
	
	memset(NextCommand, 0, HM10BufferSize);
	HM10NewDataAvailable = 0;
	
	sendStatus();
	
	/**float dist;
	while(!ultrasonicSensorNewDataAvailable);
	ultrasonicSensorNewDataAvailable=0;
	dist = (ultrasonicSensorFallingCaptureTime - ultrasonicSensorRisingCaptureTime) / 58.0;

	if(dist>35){
			LED1_On();
			LED2_On();
			LED3_On();
			LED4_On();
	} else if(dist>25){
			LED1_Off();
			LED2_On();
			LED3_On();
			LED4_On();
	}else if(dist>15){
			LED1_Off();
			LED2_Off();
			LED3_On();
			LED4_On();
	}else if(dist>5){
			LED1_Off();
			LED2_Off();
			LED3_Off();
			LED4_On();
	}
	else{
			LED1_Off();
			LED2_Off();
			LED3_Off();
			LED4_Off();
	}
	**/

}	

int main() {
	init();
	
	while(1) {
		update();
	}
}


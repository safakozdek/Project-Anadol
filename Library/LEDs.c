#include "LEDs.h"
#include "PWM.h"

const uint8_t LED_PWM_CHANNELS[] = {1,2,3,4};

void Init_LED() {
	IOCON_BACK_LEFT_LED = (IOCON_BACK_LEFT_LED | (1 << 1 | 1)) & ~(1 << 2); //011 PWM
	IOCON_BACK_RIGHT_LED = (IOCON_BACK_RIGHT_LED | (1 << 1 | 1)) & ~(1 << 2); 
	IOCON_FRONT_LEFT_LED = (IOCON_FRONT_LEFT_LED | (1 << 1 | 1)) & ~(1 << 2); 
	IOCON_FRONT_RIGHT_LED = (IOCON_FRONT_RIGHT_LED | (1 << 1 | 1)) & ~(1 << 2); 

	PWM_Init(LED_PWM_INDEX, LED_PWM_CHANNELS, 4);
}


void turnOffLED() {
	// Turn off all LEDs.
	PWM_Write(LED_PWM_INDEX, FRONT_LEFT_LED, 0);
	PWM_Write(LED_PWM_INDEX, FRONT_RIGHT_LED, 0);
	PWM_Write(LED_PWM_INDEX, BACK_LEFT_LED, 0);
	PWM_Write(LED_PWM_INDEX, BACK_RIGHT_LED, 0);
}


void frontLED(){
  //Turn on front LEDs, others off.
	PWM_Cycle_Rate(LED_PWM_INDEX, 20);
	
	PWM_Write(LED_PWM_INDEX, BACK_LEFT_LED, 0);
	PWM_Write(LED_PWM_INDEX, BACK_RIGHT_LED, 0);
	PWM_Write(LED_PWM_INDEX, FRONT_LEFT_LED, 100);
	PWM_Write(LED_PWM_INDEX, FRONT_RIGHT_LED, 100);
}

void backLED(){
  //Turn on back LEDs, others off.
	PWM_Cycle_Rate(LED_PWM_INDEX, 20);
	
	PWM_Write(LED_PWM_INDEX, FRONT_LEFT_LED, 0);
	PWM_Write(LED_PWM_INDEX, FRONT_RIGHT_LED, 0);
	PWM_Write(LED_PWM_INDEX, BACK_LEFT_LED, 100);
	PWM_Write(LED_PWM_INDEX, BACK_RIGHT_LED, 100);
}

void leftLED(){
  //LEDs at left blink twice a second, others off.
	PWM_Cycle_Rate(LED_PWM_INDEX, 500);
	
	PWM_Write(LED_PWM_INDEX, FRONT_LEFT_LED, 50);
	PWM_Write(LED_PWM_INDEX, FRONT_RIGHT_LED, 0);
	PWM_Write(LED_PWM_INDEX, BACK_LEFT_LED, 50);
	PWM_Write(LED_PWM_INDEX, BACK_RIGHT_LED, 0);
}

void rightLED(){
  //LEDs at right blink twice a second, others off.
	PWM_Cycle_Rate(LED_PWM_INDEX, 500);
	
	PWM_Write(LED_PWM_INDEX, FRONT_LEFT_LED, 0);
	PWM_Write(LED_PWM_INDEX, FRONT_RIGHT_LED, 50);
	PWM_Write(LED_PWM_INDEX, BACK_LEFT_LED, 0);
	PWM_Write(LED_PWM_INDEX, BACK_RIGHT_LED, 50);
}

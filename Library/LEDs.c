#include "LEDs.h"

void LED_Init() {
	IOCON_BACK_LEFT_LED = (IOCON_BACK_LEFT_LED | (1 << 1 | 1)) & ~(1 << 2); //011 PWM
	IOCON_BACK_RIGHT_LED = (IOCON_BACK_RIGHT_LED | (1 << 1 | 1)) & ~(1 << 2); 
	IOCON_FRONT_LEFT_LED = (IOCON_FRONT_LEFT_LED | (1 << 1 | 1)) & ~(1 << 2); 
	IOCON_FRONT_RIGHT_LED = (IOCON_FRONT_RIGHT_LED | (1 << 1 | 1)) & ~(1 << 2); 

	PCONP |= (1<<5);
	
	PWM0->PCR |= ((1<<9) | (1<<10) | (1<<11) | (1<<12));
	
	//Reset The PWM Timer Counter and The PWM Prescale Counter on the Next Positive Edge of PCLK
	PWM0->TCR |= (1<<1); 
	
	//Configure MR0 register for giving pulse every 20 ms.
	PWM0->MR0 = 1200000;
	
	//Reset TC, when MR0 matches TC (Reset on MR0)
	PWM0->MCR |= (1<<1);
	
	//Enable PWM Match 0 Latch.
	PWM0->LER |= 1;
	
	//Enable Counter, Enable PWM and Clear Reset on the PWM
	PWM0->TCR |= (1 | (1<<3)); 
	PWM0->TCR &= ~(1<<1);
	
	LED_Write(0, FRONT_RIGHT);
	LED_Write(0, FRONT_LEFT);
	LED_Write(0, BACK_LEFT);
	LED_Write(0, BACK_RIGHT);
}

void LED_Cycle_Rate(uint32_t period_In_Cycles) {
	//Write a formula that changes the MR0 register value for a given parameter.
	//For example, if period_In_Cycles 20, configure MR0 register for giving pulse every 20 ms.
	//if period_In_Cycles 1000, configure MR0 register for giving pulse every 1000ms etc.
	PWM0->MR0 = (CLOCK_RATE / 1000) * period_In_Cycles;
	PWM0->LER |= 1 << 0;
}

void LED_Write(uint32_t T_ON, uint8_t LED_MR) {	
	if(T_ON > 100) {
		T_ON = 100;
	}
	
	//Write a formula to calculate the match register for Green LED.
	//Store the value in T_ON variable again(T_ON = ???)
	T_ON = (T_ON * PWM0->MR0) / 100; 
	
	if (T_ON == PWM0->MR0) {
		T_ON++;
	}
	
	if(LED_MR == 1){
		PWM0->MR1 = T_ON;
		PWM0->LER |= 1 << 1;
	}else if(LED_MR == 2){
		PWM0->MR2 = T_ON;
		PWM0->LER |= 1 << 2;
	}else if(LED_MR == 3){
		PWM0->MR3 = T_ON;
		PWM0->LER |= 1 << 3;
	}else if(LED_MR == 4){
		PWM0->MR4 = T_ON;
		PWM0->LER |= 1 << 4;
	}else if(LED_MR == 5){
		PWM0->MR5 = T_ON;
		PWM0->LER |= 1 << 5;
	}else if(LED_MR == 6){
		PWM0->MR6 = T_ON;
		PWM0->LER |= 1 << 6;
	}
}

void frontRightLEDOn(uint16_t T_ON){
	LED_Write(T_ON, FRONT_RIGHT);
}

void frontLeftLEDOn(uint16_t T_ON){
	LED_Write(T_ON, FRONT_LEFT);
}

void backRightLEDOn(uint16_t T_ON){
	LED_Write(T_ON, BACK_RIGHT);
}

void backLeftLEDOn(uint16_t T_ON){
	LED_Write(T_ON, BACK_LEFT);
}

void frontRightLEDOff(){
	LED_Write(0, FRONT_RIGHT);
}

void frontLeftLEDOff(){
	LED_Write(0, FRONT_LEFT);
}

void backRightLEDOff(){
	LED_Write(0, BACK_RIGHT);
}

void backLeftLEDOff(){
	LED_Write(0, BACK_LEFT);
}

void turnOffLED() {
	// Turn off all LEDs.
	LED_Cycle_Rate(20);
	frontLeftLEDOff();
	frontRightLEDOff();
	backLeftLEDOff();
	backRightLEDOff();
}

void forwardLED(){
  //Turn on front LEDs, others off.
	LED_Cycle_Rate(20);
	backLeftLEDOff();
	backRightLEDOff();
	frontLeftLEDOn(100);
	frontRightLEDOn(100);
}

void backLED(){
  //Turn on back LEDs, others off.
	LED_Cycle_Rate(20);
	frontLeftLEDOff();
	frontRightLEDOff();
	backLeftLEDOn(100);
	backRightLEDOn(100);
}

void leftLED(){
  //LEDs at left blink twice a second, others off.
	LED_Cycle_Rate(500);
  frontRightLEDOff();
	backRightLEDOff();
	frontLeftLEDOn(50);
	backLeftLEDOn(50);
}

void rightLED(){
  //LEDs at right blink twice a second, others off.
	LED_Cycle_Rate(500);
	frontLeftLEDOff();
	backLeftLEDOff();
	frontRightLEDOn(50);
	backRightLEDOn(50);
}

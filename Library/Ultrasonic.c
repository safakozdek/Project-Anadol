#include "Ultrasonic.h"
#include "Motor.h"
#include "LEDs.h"

uint32_t ultrasonicSensorsRisingCaptureTime[2] = {0};
uint32_t ultrasonicSensorsFallingCaptureTime[2] = {0};

uint32_t ultrasonicSensorsDurations[2] = {0};
uint8_t ultrasonicSensorsNewDataAvailable[2] = {0};

uint8_t ultrasonicSensorTriggerStart = 0;
uint8_t ultrasonicSensorsCaptureRisingEdge[2] = {0};

uint8_t isTurnComplete = 0; 

void Ultrasonic_Init() {
	IOCON_TRIGGER |= 0x03;
	IOCON_ECHO_1 |= 0x03;
	IOCON_ECHO_2 |= 0x03;
}

void Ultrasonic_Trigger_Timer_Init() {
	// Turn on Timer2.
	PCONP |= 1<<22 ;
	
	// Change the mode of Timer2 to Timer Mode.
	TIMER2->CTCR &= ~(0x3);
	
	TIMER2->TCR &= ~(1 << 0);
	
	TIMER2->TCR |= (1 << 1);
	
	// Change PR Register value for 1 microsecond incrementing
	TIMER2->PR = 59;
	
	// Write the Correct Configuration for EMR (LOW output value of Trigger Pin when match occurs and Initial value is LOW)
	TIMER2->EMR = (TIMER2->EMR | 1<<10)  & ~(1<<11 | 1<<3);
	
	// Capture for interrupt sensor (interrupt on falling edge)
	TIMER2->CCR = 5;
	
	NVIC_EnableIRQ(TIMER2_IRQn);
	
	NVIC_SetPriority(TIMER2_IRQn,5);
	
	NVIC_ClearPendingIRQ(TIMER2_IRQn);
}

void Ultrasonic_Capture_Timer_Init() {
	// Turn on Timer3
	PCONP |= 1<<23 ;
	
	// Change the mode of Timer3 to Timer Mode
	TIMER3->CTCR &= ~(0x3);
	
	TIMER3->TCR &= ~(1 << 0);
	TIMER3->TCR |= (1 << 1);
	
	// Change PR Register value for 1 microsecond incrementing
	TIMER3->PR = 59;
	
	// Change CCR value for getting Interrupt when Rising Edge Occur for CAP 1 and 0
	TIMER3->CCR |= (1 << 6) - 1;
	
	TIMER3->TCR &= ~(1 << 1);
	TIMER3->TCR |= (1 << 0);

	NVIC_EnableIRQ(TIMER3_IRQn);
}

void Ultrasonic_Start_Trigger_Timer() {
	//Change output value of Trigger Pin as HIGH
	TIMER2->EMR |= 1<<3;
	
	//Give correct value to corresponding MR Register for 10 microsecond
	TIMER2->MR3 = 10;
	
	
	//Enable interrupt for MR3 register, if MR3 register matches the TC.
	TIMER2->MCR = (TIMER2->MCR | 1<<9) & ~(1<<10);

	//Remove the reset on counters of Timer2.
	//Enable Timer Counter and Prescale Counter for counting.
	TIMER2->TCR =(TIMER2->TCR | (1 << 0)) & ~(1<<1);
	
}

void TIMER2_IRQHandler() {
	//Write HIGH bit value to IR Register for Corresponding Interrupt
	if (TIMER2->IR & 1 << 3) {
		// Time to trigger the ultrasonic sensor
 		
		TIMER2->IR |= 1 << 3;

		if(ultrasonicSensorTriggerStart == 0) {
			//Change MR3 Register Value for Suggested Waiting
			TIMER2->MR3 = 60000 + TIMER2->TC;
			
			ultrasonicSensorTriggerStart = 1;
		}
		else {
			TIMER2->EMR |= (1 << 3);
			TIMER2->MR3 = 10 + TIMER2->TC;
			
			ultrasonicSensorTriggerStart = 0;
		}
	} else if (TIMER2->IR & 1 << 4) {
		TIMER2->IR |= 1 << 4;
		
		++tickCount;
		
		if (rotateUntilTick > 0 && tickCount >= rotateUntilTick) {
			isTurnComplete = 1;
			rotateUntilTick = 0;
		}
	}
}

void TIMER3_IRQHandler() {
	uint8_t sourceIndex = (TIMER3->IR & (1 << 4)) ? 0 : 1;
	
	TIMER3->IR |= 1 << (sourceIndex + 4);
	
	if(ultrasonicSensorsCaptureRisingEdge[sourceIndex] == 1) {
		ultrasonicSensorsRisingCaptureTime[sourceIndex] = (sourceIndex == 0 ? TIMER3->CR0 : TIMER3->CR1);
		
		if (sourceIndex == 1)
			LPC_TIM3->CCR = (LPC_TIM3->CCR | (1 << 4) | (1 << 5)) & ~(1 << 3);
		else
			LPC_TIM3->CCR = (LPC_TIM3->CCR | (1 << 1) | (1 << 2)) & ~(1 << 0);

		ultrasonicSensorsCaptureRisingEdge[sourceIndex] = 0;
	}
	else {
		ultrasonicSensorsFallingCaptureTime[sourceIndex] = (sourceIndex == 0 ? TIMER3->CR0 : TIMER3->CR1);
		ultrasonicSensorsDurations[sourceIndex] = (ultrasonicSensorsFallingCaptureTime[sourceIndex] - ultrasonicSensorsRisingCaptureTime[sourceIndex]);
		ultrasonicSensorsNewDataAvailable[sourceIndex] = 1;

		if (sourceIndex == 1)
			LPC_TIM3->CCR = (LPC_TIM3->CCR | (1 << 3) | (1 << 5)) & ~(1 << 4);
		else
			LPC_TIM3->CCR = (LPC_TIM3->CCR | (1 << 0) | (1 << 2)) & ~(1 << 1);

		ultrasonicSensorsCaptureRisingEdge[sourceIndex] = 1;
	}
}


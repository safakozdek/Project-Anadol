#ifndef LEDS_H
#define LEDS_H

#include "LPC407x_8x_177x_8x.h"

#include "SystemStructures.h"

#define IOCON_BACK_RIGHT_LED	*((volatile uint32_t*)( 0x4002C088))
#define IOCON_BACK_LEFT_LED	*((volatile uint32_t*)( 0x4002C08C))
#define IOCON_FRONT_RIGHT_LED	*((volatile uint32_t*)( 0x4002C094))
#define IOCON_FRONT_LEFT_LED	*((volatile uint32_t*)( 0x4002C098))
	
#define LED_PWM_INDEX 0
			
typedef enum{
	FRONT_RIGHT_LED = 3, 
	FRONT_LEFT_LED = 4,
	BACK_RIGHT_LED = 1, 
	BACK_LEFT_LED = 2,
} LED_MR_NO;

void Init_LED(void);

void turnOffLED(void);
void frontLED(void);
void backLED(void);
void leftLED(void);
void rightLED(void);

#endif

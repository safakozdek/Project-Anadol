#ifndef LEDS_H
#define LEDS_H

#include "LPC407x_8x_177x_8x.h"

#include "SystemStructures.h"

typedef struct
{
  volatile	uint32_t IR;
  volatile	uint32_t TCR;
  volatile	uint32_t TC;
  volatile	uint32_t PR;
  volatile	uint32_t PC;
  volatile	uint32_t MCR;
  volatile	uint32_t MR0;
  volatile	uint32_t MR1;
  volatile	uint32_t MR2;
  volatile	uint32_t MR3;
  volatile	uint32_t CCR;
  volatile  uint32_t CR0;
  volatile  uint32_t CR1;
  volatile  uint32_t CR2;
  volatile  uint32_t CR3;
						uint32_t RESERVED0;
  volatile	uint32_t MR4;
  volatile	uint32_t MR5;
  volatile	uint32_t MR6;
  volatile	uint32_t PCR;
  volatile	uint32_t LER;
						uint32_t RESERVED1[7];
  volatile	uint32_t CTCR;
} PWM_TypeDef;

#define IOCON_BACK_RIGHT_LED	*((volatile uint32_t*)( 0x4002C088))
#define IOCON_BACK_LEFT_LED	*((volatile uint32_t*)( 0x4002C08C))
#define IOCON_FRONT_RIGHT_LED	*((volatile uint32_t*)( 0x4002C094))
#define IOCON_FRONT_LEFT_LED	*((volatile uint32_t*)( 0x4002C098))
			
#define PWM0_BASE	0x40014000
#define PWM1_BASE	0x40018000

#define PWM0	((PWM_TypeDef*) PWM0_BASE)
#define PWM1	((PWM_TypeDef*) PWM1_BASE)

#define CLOCK_RATE ((uint32_t) 60000000)

typedef enum{
	FRONT_RIGHT = 3, 
	FRONT_LEFT = 4,
	BACK_RIGHT = 1, 
	BACK_LEFT = 2,
} LED_MR_NO;

void LED_Init(void);
void LED_Cycle_Rate(uint32_t period_In_Cycles);
void LED_Write(uint32_t T_ON, uint8_t LED_MR);
void frontRightLEDOn(uint16_t T_ON);
void frontLeftLEDOn(uint16_t T_ON);
void backRightLEDOn(uint16_t T_ON);
void backLeftLEDOn(uint16_t T_ON);
void frontRightLEDOff(void);
void frontLeftLEDOff(void);
void backLeftLEDOff(void);
void backRightLEDOff(void);
void turnOffLED(void);
void forwardLED(void);
void backLED(void);
void leftLED(void);
void rightLED(void);

#endif

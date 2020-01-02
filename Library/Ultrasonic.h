#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "GPIO.h"
#include "Timer.h"
#include "SystemStructures.h"

//Write IOCON Register Address of Trigger Pin.
#define IOCON_TRIGGER_ADDRESS	0x4002C024
#define IOCON_TRIGGER	*((volatile uint32_t*)(IOCON_TRIGGER_ADDRESS))

//Write IOCON Register Address of Echo Pin.
#define IOCON_ECHO_1_ADDRESS	0x4002C060
#define IOCON_ECHO_1	*((volatile uint32_t*)(IOCON_ECHO_1_ADDRESS))
	
#define IOCON_ECHO_2_ADDRESS	0x4002C05C
#define IOCON_ECHO_2	*((volatile uint32_t*)(IOCON_ECHO_2_ADDRESS))

#define BACK_ULTRASONIC_INDEX 1
#define FRONT_ULTRASONIC_INDEX 0

extern uint32_t ultrasonicSensorsDurations[2];
extern uint8_t ultrasonicSensorsNewDataAvailable[2];
extern uint8_t isTurnComplete;

void Ultrasonic_Init(void);
void Ultrasonic_Trigger_Timer_Init(void);
void Ultrasonic_Capture_Timer_Init(void);

void Ultrasonic_Start_Trigger_Timer(void);

#endif

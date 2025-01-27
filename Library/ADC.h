#ifndef ADC_H
#define ADC_H

#include "LPC407x_8x_177x_8x.h"

#include "SystemStructures.h"

typedef struct {
  volatile	uint32_t CR;
  volatile	uint32_t GDR;
						uint32_t RESERVED0;
  volatile	uint32_t INTEN;
  volatile	uint32_t DR[8];
  volatile	uint32_t STAT;
  volatile	uint32_t TRM;
} ADC_TypeDef;

#define ADC_CLOCK_FREQUENCY 1000000

//Write the IOCON address of Analog Pin
#define	ANALOG_LDR1_IOCON_ADDRESS	0x4002C064
#define	ANALOG_LDR2_IOCON_ADDRESS	0x4002C068
#define	ANALOG_POT_IOCON_ADDRESS	0x4002C0F8

#define ANALOG_LDR1_IOCON	*((volatile uint32_t*)(ANALOG_LDR1_IOCON_ADDRESS))
#define ANALOG_LDR2_IOCON	*((volatile uint32_t*)(ANALOG_LDR2_IOCON_ADDRESS))
#define ANALOG_POT_IOCON	*((volatile uint32_t*)(ANALOG_POT_IOCON_ADDRESS))

//Write the max value of ADC.
#define ADC_MAX_VALUE 0x00000FFF

//Define a ADC_CLKDIV variable for given ADC_CLOCK_FREQUENCY.
#define ADC_CLKDIV 59

#define ADC_BASE	0x40034000
#define ADC	((ADC_TypeDef*) ADC_BASE)

extern uint32_t ADC_Last[8];
extern uint8_t ADC_New_Data_Available[8];

#define LDR1_CHANNEL_INDEX 3
#define LDR2_CHANNEL_INDEX 2

#define LEFT_LDR_CHANNEL_INDEX LDR1_CHANNEL_INDEX
#define RIGHT_LDR_CHANNEL_INDEX LDR2_CHANNEL_INDEX

#define LDR_BRIGHT_LIMIT 1500

#define POTENTIOMETER_CHANNEL_INDEX 4

void ADC_Init(void);
void ADC_Start(void);
void ADC_Stop(void);
uint32_t ADC_GetLastValue(uint8_t);

#endif

#include "ADC.h"
#include "Motor.h"

uint32_t ADC_Values[8] = {0};
uint8_t ADC_New_Data_Available[8] = {0};

void ADC_Init() {
	//Change the function value of pin to ADC.
	//Change the mode value of pin to mode which should be selected if Analog mode is used.
	//Change Analog/Digital mode of pin to Analog.
	ANALOG_LDR1_IOCON = ((ANALOG_LDR1_IOCON & ~(31)) | 1) & ~(1<<7);
	ANALOG_LDR2_IOCON = ((ANALOG_LDR2_IOCON & ~(31)) | 1) & ~(1<<7);
	ANALOG_POT_IOCON = ((ANALOG_POT_IOCON & ~(31)) | 3) & ~(1<<7);
	
	//Turn on ADC.
	PCONP |= 1<<12;	
	
	//Set the CLKDIV and make the A/D converter operational without Burst mode.
	//Make the A/D converter operational
	//Make sure conversions are software controlled and require 31 clocks (Do not use Burst mode)
	//Configure CR SEL bits for sampled and converting corresponding pin.
	ADC->CR = (((ADC->CR & ~(((1<<8)-1)<<8)) | (ADC_CLKDIV << 8) | (1<<21)) & ~((1 << 8) - 1)) | (0x7 << 2) | (1 << 16);
	
	//Enable interrupt for corresponding pin.
	ADC->INTEN |= 0x7 << 2;
	
	//Enable ADC_IRQn (Interrupt Request).
	NVIC_EnableIRQ(ADC_IRQn);
}

void ADC_Start () {
	//Write a code for starting A/D conversion on a rising edge on the TIMER 0 MATCH 1.
	ADC->CR = (ADC->CR & ~(0x7<<24));

}

uint32_t ADC_GetLastValue(uint8_t index) {
	ADC_New_Data_Available[index] = 0;
	
	return ADC_Values[index];
}

void ADC_IRQHandler() {
	uint32_t doneFlags = ADC->STAT & ((1 << 8) - 1), i;
	
	ADC->GDR &= ~((uint32_t)1 << 31);
	
	for (i = 0; i < 8; ++i) {
		if (doneFlags & (1 << i)) {
			ADC_Values[i] = (ADC->DR[i] >> 4) & ((1<<12) - 1);
			ADC_New_Data_Available[i] = 1;
			
			/*if ((i == LDR1_CHANNEL_INDEX || i == LDR2_CHANNEL_INDEX)) {
				if (ADC_Values[i] < LDR_BRIGHT_LIMIT) {
					Pause_Motor(0);
				} else {
					Resume_Motor(0);
				}		 
			}*/
		}
	}		
}

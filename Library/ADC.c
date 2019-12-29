#include "ADC.h"

uint32_t ADC_Last = 0;
uint8_t ADC_New_Data_Available = 0;

void ADC_Init() {
	//Change the function value of pin to ADC.
	//Change the mode value of pin to mode which should be selected if Analog mode is used.
	//Change Analog/Digital mode of pin to Analog.
	ANALOG_PIN_IOCON = ((ANALOG_PIN_IOCON & ~(31)) | 1) & ~(1<<7);
	//Turn on ADC.
	PCONP |= 1<<12;	
	//Set the CLKDIV and make the A/D converter operational without Burst mode.
	//Make the A/D converter operational
	//Make sure conversions are software controlled and require 31 clocks (Do not use Burst mode)
	//Configure CR SEL bits for sampled and converting corresponding pin.
	ADC->CR = (((ADC->CR & ~(((1<<9)-1)<<8)) | (ADC_CLKDIV << 8) | (1<<21)) & ~((1 << 8) - 1)) | (1 << 2);



	
	
	//Enable interrupt for corresponding pin.
	ADC->INTEN |= 1<<2;
	
	//Enable ADC_IRQn (Interrupt Request).
	NVIC_EnableIRQ(ADC_IRQn);
}

void ADC_Start () {
	//Write a code for starting A/D conversion on a rising edge on the TIMER 0 MATCH 1.
	ADC->CR = (ADC->CR & ~(0x7<<24)) | (0x4<<24);

}

uint32_t ADC_GetLastValue() {
	ADC_New_Data_Available = 0;
	return ADC_Last;
}

void ADC_IRQHandler() {
	ADC->GDR &= ~((uint32_t)1 << 31);
	
	//Write the converted data (only the converted data) to ADC_Last variable.	
	ADC_Last = (ADC->DR[2] >> 4) & ((1<<12) - 1);

	
	ADC_New_Data_Available = 1;
}

#include "PWM.h"
#include "wait.h"

PWM_TypeDef* const PWMs[] = {PWM0, PWM1};

void PWM_Init(const uint8_t pwmIndex, const uint8_t channels[], const uint8_t n_channels) {
	PWM_TypeDef *const PWMX = PWMs[pwmIndex];
	uint8_t i;
	uint32_t PCR;
	
	//Change the function of the pin in here:
	PCONP |= (1 << (5 + pwmIndex));
	
	PCR = PWMX->PCR;
	//Enable PWM output for corresponding pin.
	for (i = 0; i < n_channels; ++i)
			PCR |= (1 << (8 + channels[i])) & ~(1 << channels[i]);
	
	PWMX->PCR = PCR;
	
	PWMX->TCR = 1 << 1;
	
	//PWMX->PR = 9; 
	
	//Configure MR0 register for a period of 20 ms
	PWMX->MR0_3[0] = 1200000;
	
	PWMX->MCR = 1 << 1;
	
	PWMX->LER = 1 << 0;
	
	PWMX->TCR = (1 << 0 | 1 << 3);
	
	for (i = 0; i < n_channels; ++i)
		PWM_Write(pwmIndex, channels[i], 0);
}

void PWM_Cycle_Rate(uint8_t pwmIndex, uint32_t period) {
	//Write a formula that changes the MR0 register value for a given parameter.
	PWM_TypeDef *const PWMX = PWMs[pwmIndex];
	
	PWMX->MR0_3[0] = (PERIPHERAL_CLOCK_FREQUENCY / (PWMX->PR + 1) / 1000) * period;

	PWMX->LER = (1 << 8) - 1;
	//wait(10);
}

void PWM_Write(uint8_t pwmIndex, uint8_t channelIndex, uint32_t T_ON) {	
	PWM_TypeDef * PWMX = PWMs[pwmIndex];
	volatile uint32_t *pMR = channelIndex > 3 ? &PWMX->MR4_6[channelIndex - 4] : &PWMX->MR0_3[channelIndex]; 
	
	if(T_ON > 100) {
		T_ON = 100;
	}
	
	*pMR = (T_ON * PWMX->MR0_3[0]) / 100;
	PWMX->LER = (1 << 8) - 1;
	//wait(10);
}

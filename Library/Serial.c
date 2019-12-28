#include "Serial.h"

volatile char serialReceivedCharacter = 0;
volatile uint8_t serialNewDataAvailable = 0;
char* serialTransmitData = 0;
uint8_t serialTransmitCompleted = 0;


void Serial_Init() {
	//Change the function of TX and RX pins for UART.
	Serial_UART_TX_PIN = ((Serial_UART_TX_PIN & ~(0x7)) | 0x1);
	Serial_UART_RX_PIN = ((Serial_UART_RX_PIN & ~(0x7)) | 0x1);
	
	//Turn on UART0.
	PCONP |= 1 << 3;
	
	//Enable FIFO for UART0.
	Serial_UART->FCR |= 1<<0;
	
	//In order to change the DLM, DLL and FDR values, Write correct code for enabling the access to Divisor Latches.
	Serial_UART->LCR |= 1<<7;
	
	//Write correct DLM, DLL and FDR values for 115200 baudrate
	Serial_UART->DLM = 0x00; 
	Serial_UART->DLL = 0x13; 
	Serial_UART->FDR = 0x05 << 0 | 0x07 << 4;
	
	//Write correct code for disabling the access to Divisor Latches.
	//Change LCR register value for 8-bit character transfer, 1 stop bits and Even Parity.
	Serial_UART->LCR = ((Serial_UART->LCR & ~((1<<6) - 1)) | 0x1b) &  ~(1<<7);
				
	//Enable the Receive Data Available and THRE Interrupt.
	Serial_UART->IER |= 0x3;
	
	//Enable UART0_IRQn Interrupt.
	NVIC_EnableIRQ(UART0_IRQn);
	
	//Set UART0_IRQn Priority to 5.
	NVIC_SetPriority(UART0_IRQn,5);
}

void UART0_IRQHandler() {	
	uint32_t currentInterrupt = (Serial_UART->IIR >> 1)&(0x7);
	
	//First if statement is for Receive Data Available interrupt.
	//When Receive Data Available, the code will enter to the first if condition.
	if(currentInterrupt == 0x2) {
		serialReceivedCharacter = Serial_ReadData();
		serialNewDataAvailable = 1;
	}
	//Second if statement is for THRE interrupt
	//When THRE interrupt is handled, the code will enter to the second if condition.
	else if(currentInterrupt == 0x1) {
		if(*serialTransmitData > 0) {
			Serial_WriteData(*serialTransmitData++);
		}
		else {
			serialTransmitCompleted = 1;
		}
	}
}

char Serial_ReadData() {
	return Serial_UART->RBR;
}

void Serial_WriteData(const char data) {
	serialTransmitCompleted = 0;
	Serial_UART->THR = data;
}


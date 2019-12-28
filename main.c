#include "LPC407x_8x_177x_8x.h"

#include <stdio.h> 
#include <string.h> 

#include "Library/Serial.h"
#include "Library/HM10.h"

#define UART_READ_BUFFER_SIZE 512
#define UART_WRITE_BUFFER_SIZE 512

char uartReadBuffer[UART_READ_BUFFER_SIZE] = "";
char uartWriteBuffer[UART_WRITE_BUFFER_SIZE] = ""; 

void init() {	
	Serial_Init();
	
	HM10_Init();
	HM10_SendCommand("AT+NAMEANADOL");
	HM10_SendCommand("AT+RESET");
}

char readWhenAvailable() {
	while (!serialNewDataAvailable);
	
	serialNewDataAvailable = 0;
	
	return serialReceivedCharacter;
}


void update_old() {
	unsigned uartReadBufferWriteTo = 0, HM10ResponseStart = 0, uartWriteTo = 0;
	memset(uartReadBuffer, 0, UART_READ_BUFFER_SIZE);
	memset(uartWriteBuffer, 0, UART_WRITE_BUFFER_SIZE);
	
	while (1) {
		char read = readWhenAvailable();
		
		if (read == 13)
			break;
		
		uartReadBuffer[uartReadBufferWriteTo++] = read;
	}

	uartReadBuffer[uartReadBufferWriteTo++] = '\r';
	uartReadBuffer[uartReadBufferWriteTo++] = '\n';
	
	HM10_ClearBuffer();
	HM10_SendCommand(uartReadBuffer);
	
	while (!HM10NewDataAvailable);
	HM10NewDataAvailable = 0;
	
	//uartWriteBuffer[uartWriteTo++] = '\r';
	//uartWriteBuffer[uartWriteTo++] = '\n';
	
	serialTransmitData = HM10Buffer;
	Serial_WriteData(*serialTransmitData++);
	while(!serialTransmitCompleted);
}

void update() {
	while (!HM10NewDataAvailable);
	HM10NewDataAvailable = 0;
	HM10_SendCommand(NextCommand);
}	

int main() {
	init();
	
	while(1) {
		update_old();
	}
}

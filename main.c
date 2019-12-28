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
}

char readWhenAvailable() {
	while (!serialNewDataAvailable);
	
	serialNewDataAvailable = 0;
	
	return serialReceivedCharacter;
}

void update() {
	while (!HM10NewDataAvailable);
	HM10_SendCommand(NextCommand);
	memset(NextCommand, 0, HM10BufferSize);
	HM10NewDataAvailable = 0;
}	

int main() {
	init();
	
	while(1) {
		update();
	}
}


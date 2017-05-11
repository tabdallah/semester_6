#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "TA_Header_S2017.h"  /* my macros and constants */
#include "CAN.h"

// Code from AN3034 app note

// Function to initialize CAN module
void CANInit(void) {
	CANCTL0 = CAN_INIT;	// Enter initialization mode
	while (!(CANCTL1 & CAN_INIT)) {
		// Wait for initialization mode acknowledge
	};

	CANCTL1 = CAN_NORMAL;	// Enable module in loopback mode with oscillator clock
	CANBTR0 = BTR0_125K;		// Set baud rate to 125KBaud
	CANBTR1 = BTR1_125K;		// Set baud rate to 125KBaud

	CANIDAC = CAN_FILTER_16b;	// Define four 16-bit filters
	
	// 16-bit filter for ID 0x100
	CANIDAR0 = ACC_CODE_ID100_HIGH;
	CANIDMR0 = MASK_CODE_ST_ID_HIGH;
	CANIDAR1 = ACC_CODE_ID100_LOW;
	CANIDMR1 = MASK_CODE_ST_ID_LOW;

	CANCTL0 = CAN_START;	// Exit initialization mode
	while ((CANCTL1 & CAN_START) != 0) {
		// Wait for normal mode acknowledge
	};
}

// Function to send CAN frame
unsigned char CANSendFrame(unsigned long id, unsigned char priority, unsigned char length, unsigned char *txdata) {

	unsigned char txbuffer;	// To store the selected buffer for transmitting
	unsigned char index;    // Index into the data array

	// Check if transmit buffer is full
	if (!CANTFLG) {
		return CAN_ERR_BUFFER_FULL;
	}

	CANTBSEL = CANTFLG; 	// Select lowest empty buffer
	txbuffer = CANTBSEL;	// Backup selected buffer

	// Load CAN ID to IDR register
	*((unsigned long *) ((unsigned long)(&CANTXIDR0))) = id;

	// Load message data to data segment registers
	for (index=0; index<length; index++) {
		*(&CANTXDSR0 + index) = txdata[index]; // Load data to Tx buffer data segment registers
	}

	CANTXDLR = length;		// Set DLC
	CANTXTBPR = priority;	// Set priority
	CANTFLG = txbuffer; 	// Start transmission

	while ( (CANTFLG & txbuffer) != txbuffer);	// Wait for transmit to complete
}


// ISR for receiving CAN frame
interrupt 38 CANRxISR(void)
{
	unsigned char length, index;
	unsigned char rxdata[8];
	length = (CANRXDLR & 0x0F);
	for (index=0; index<length; index++)
	rxdata[index] = *(&CANRXDSR0 + index); /* Get received data */
	CANRFLG = 0x01;	// Claer Rx flag
}
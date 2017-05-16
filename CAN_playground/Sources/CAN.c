#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "TA_Header_S2017.h"  /* my macros and constants */
#include "CAN.h"
#include "FIFO.h"
#include <stdlib.h>

// Code from AN3034 app note (but not really because that code was hot garbage)

//;**************************************************************
//;*                 configureCAN()
//;*  Configures & enables the CAN controller
//;**************************************************************  
void configureCAN(void) {
	CANCTL0 = CAN_INIT;	// Enter initialization mode
	while (!(CANCTL1 & CAN_INIT)) {
		// Wait for initialization mode acknowledge
	};

	CANCTL1 = CAN_NORMAL;		// Enable module in normal mode with bus clock
	//CANCTL1 = CAN_LOOPBACK;		// Enable module in loopback mode with bus clock
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

//;**************************************************************
//;*                 TxCAN(id, priority, length, *txdata)
//;*  Outputs a CAN frame using polling
//;**************************************************************   
unsigned char TxCAN(unsigned long id, unsigned char priority,
	unsigned char length, unsigned char *txdata) {

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

	return CAN_ERR_NONE;
}


//;**************************************************************
//;*                 RxHandlerCAN()
//;*  Interrupt handler for CAN Rx
//;**************************************************************
interrupt 38 void RxHandlerCAN(void) {

	// Initialzie a new node to be added to the FIFO queue
	link message = malloc(sizeof(node));
  
  unsigned char i;	      // Loop counter
	unsigned long ID0, ID1;   // To read CAN ID registers and manipulate 11-bit ID's into a single number

	// Read CAN ID & simplify it to a single number
	ID0 = (CANRXIDR0 << 3);
	ID1 = (CANRXIDR1 >> 5);	
	message->Data.ID = (0x0FFF) & (ID0 | ID1);
	
	// Read DLC
	message->Data.DLC = LO_NYBBLE(CANRXDLR);

	// Malloc space for the data based on DLC
	message->Data.DATA = malloc(message->Data.DLC);

	// Read data one byte at a time
	for (i=0; i < message->Data.DLC; i++) {
		*(message->Data.DATA + i) = *(&CANRXDSR0 + i);
	}

	// Add the new node to the FIFO queue
	AddToQueue(message);

	// Clear Rx flag
	SET_BITS(CANRFLG, CAN_RxACK);
}
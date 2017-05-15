#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "TA_Header_S2017.h"  /* my macros and constants */
#include "CAN.h"
#include "timer.h"

// Global variables to store last received CAN message
unsigned long CAN_ID;
unsigned char CAN_DLC;
unsigned char CAN_DATA[8];

// Code from AN3034 app note

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
	
	unsigned char i;	        // Loop counter
	unsigned long ID0, ID1;   // To read CAN ID registers and manipulate 11-bit ID's into a single number

	// Read CAN ID
	ID0 = (CANRXIDR0 << 3);
	ID1 = (CANRXIDR1 >> 5);	
	CAN_ID = (0x0FFF) & (ID0 | ID1);
	
	
	//CAN_ID = CANRXIDR0;

	// Read DLC
	CAN_DLC = LO_NYBBLE(CANRXDLR);

	// Read data on byte at a time
	for (i=0; i < CAN_DLC; i++) {
		CAN_DATA[i] = *(&CANRXDSR0 + i);
	}

	// Clear Rx flag
	SET_BITS(CANRFLG, CAN_RxACK);
}


interrupt 37 void ErrHandlerCAN(void) {  
  LED1_ON;
	LED2_ON;
	
	for(;;) {
		msDelay(250);
		TOGGLE_LEDS;
	}	  
}

interrupt 39 void TxHandlerCAN(void) {
  LED1_OFF;
  LED2_OFF;
  
  for(;;);
}

interrupt 36 void WkupHandlerCAN(void) {
  LED1_ON;
  LED2_ON;
  
  for(;;);
}
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "TA_Header_S2017.h"  /* my macros and constants */
#include <stdlib.h>
#include "CAN.h"
#include "timer.h"
#include "FIFO.h"
#include "LCD.h"

void main(void) {
	unsigned char errorflag = 0x00;
	unsigned char txbuff[] = "ABCDEF";
	link pTemp = NULL;

  configureCAN();
  configureTimer();
  configureLCD();
  CONFIGURE_LEDS;
  InitQueue();
  LED1_ON;
  LED2_OFF;

  LCDprintf("Program started\n");

	while (!(CANCTL0&CAN_SYNC));  // Wait for MSCAN to synchronize with the CAN bus
	SET_BITS(CANRFLG, 0xC3);      // Enable CAN Rx Interrupts
	SET_BITS(CANRIER, 0x01);      // Clear CAN Rx flag



	EnableInterrupts;


	for (;;) {
		errorflag = TxCAN(ST_ID_100, 0x00, sizeof(txbuff)-1, txbuff);
		msDelay(250);
		msDelay(250);
		msDelay(250);
		msDelay(250);
		
		if(IsQueueEmpty()) 
		{
		  // Do nothing
		} 
		else 
		{
		  pTemp = DeQueue();  // Grab a message from the Queue

      		  		  	  		  
		  LCDprintf("ID:0x%03lx DLC: %u\n%u %u %u %u %u %u %u %u", \
		    pTemp->Data.ID, pTemp->Data.DLC, *(pTemp->Data.DATA), *(pTemp->Data.DATA + 1), \
		    *(pTemp->Data.DATA + 2), *(pTemp->Data.DATA + 3), *(pTemp->Data.DATA + 4), \
		    *(pTemp->Data.DATA + 5), *(pTemp->Data.DATA + 6), *(pTemp->Data.DATA + 7));

      
		  free(pTemp->Data.DATA); // Free the memory malloc'd for data
		  free(pTemp);            // Free the memory malloc'd for the node structure				   
		}


		TOGGLE_LEDS;
	}
}
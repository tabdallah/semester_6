#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "TA_Header_S2017.h"  /* my macros and constants */
#include "CAN.h"
#include "timer.h"

void main(void) {
	unsigned char errorflag = 0x00;
	unsigned char txbuff[] = "ABCDEF";

  configureCAN();
  configureTimer();
  CONFIGURE_LEDS;
  LED1_ON;
  LED2_OFF;

	while (!(CANCTL0&CAN_SYNC));  // Wait for MSCAN to synchronize with the CAN bus
	SET_BITS(CANRFLG, 0xC3);
	SET_BITS(CANRIER, 0x01);

	EnableInterrupts;


	for (;;) {
		errorflag = TxCAN(ST_ID_100, 0x00, sizeof(txbuff)-1, txbuff);
		msDelay(250);
		TOGGLE_LEDS;
	}
}
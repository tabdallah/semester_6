#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "TA_Header_S2017.h"  /* my macros and constants */
#include "timer.h"  // Macros and constants for timer3handler.

//;**************************************************************
//;*                 configureTimer(void)
//;*  Configures the timer module with parameters for PWM operation
//;**************************************************************   
void configureTimer(void) {
  TSCR1 = TSCR1_INIT; // Turn on timer module and enable fast-clear and freeze in debug
  TSCR2 = TSCR2_INIT; // Set pre-scaler to 4 for finest resolution @50Hz PWM frequency, overflow interrupt disabled
}//end of configureTimer


//;**************************************************************
//;*                 msDelay(time)
//;*  Delay program execution by time mS (busy wait)
//;*  Delays on TC7
//;**************************************************************
void msDelay(unsigned char time) {
  // 1 TCNT tick = 0.5uS so 2000 TCNT ticks = 1mS
  volatile unsigned char count;

  SET_OC_ACTION(7,OC_OFF);     // Set TC7 to not touch the port pin
  TC7 = TCNT + TCNT_mS; // Set first OC event timer (for 1mS)
  TIOS |= TIOS_IOS7_MASK; // Enable TC7 as OC

  for(count = 0; count < time; count ++)
  {
    while(!(TFLG1 & TFLG1_C7F_MASK)); // Wait for the OC event
    TC7 += TCNT_mS;
  }

  TIOS &= LOW(~TIOS_IOS7_MASK);  // Turn off OC on TC7

}//end of msDelay


//;**************************************************************
//;*                 usDelay(time)
//;*  Delay program execution by time uS (busy wait)
//;*  Delays on TC7
//;**************************************************************
void usDelay(unsigned char time) {
  // 1 TCNT tick = 0.5uS so 2 TCNT ticks = 1uS
  volatile unsigned char count;

  SET_OC_ACTION(7,OC_OFF);     // Set TC7 to not touch the port pin
  TC7 = TCNT + TCNT_uS; // Set first OC event timer (for 1mS)
  TIOS |= TIOS_IOS7_MASK; // Enable TC7 as OC

  for(count = 0; count < time; count ++)
  {
    while(!(TFLG1 & TFLG1_C7F_MASK)); // Wait for the OC event
    TC7 += TCNT_uS;
  }

  TIOS &= LOW(~TIOS_IOS7_MASK);  // Turn off OC on TC7

}//end of usDelay
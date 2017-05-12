#include "TA_Header_S2017.h"  /* my macros and constants */

#define CAN_INIT 0x01
#define CAN_START 0x00
#define CAN_SYNC 0x10
#define CAN_LOOPBACK 0xE0	// Enable module in loopback mode with bus clock
#define CAN_NORMAL 0xC0		// Enable module in normal mode with bus clock
#define BTR0_125K 0x07
#define BTR1_125K 0x23
#define CAN_FILTER_16b 0x10		// Define four 16-bit filters

// Acceptance filter definitions
#define ACC_CODE_ID100 0x2000
#define ACC_CODE_ID100_HIGH ((ACC_CODE_ID100 & 0xFF00)>>8)
#define ACC_CODE_ID100_LOW (ACC_CODE_ID100 & 0x00FF)

// Mask code definitions
#define MASK_CODE_ST_ID 0x0007
#define MASK_CODE_ST_ID_HIGH ((MASK_CODE_ST_ID & 0xFF00)>>8)
#define MASK_CODE_ST_ID_LOW (MASK_CODE_ST_ID & 0xFF)

// CAN module errors
#define CAN_ERR_NONE 0x00
#define CAN_ERR_BUFFER_FULL 0x01


// ID definition
#define ST_ID_100 0x20000000


// Function to initialize CAN module
void configureCAN(void);

// Function to send CAN message
unsigned char TxCAN(unsigned long id, unsigned char priority, unsigned char length, unsigned char *txdata);
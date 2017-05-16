#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

// A CAN message has a CAN ID, DLC and 8 bytes of data
struct CAN_MSG
{
	unsigned long ID;
	unsigned char DLC;
	unsigned char *DATA;
};

typedef struct Node *link;	// Declaring a link as a pointer to a Node structure
typedef struct CAN_MSG msg; // Declaring a msg to be a CAN_MSG for readability

// A node has a link named 'pNext' and CAN message named 'Data'
struct Node
{
	link pNext;
	msg Data;
};

typedef struct Node node;  // Declaring node to be a Node for readability

// Function prototypes
void InitQueue(void);		// Initializes pHead and pTail to NULL
int IsQueueEmpty(void);		// Returns true if queue is IsQueueEmpty
void AddToQueue(link);	// Add a message struct to the queue
link DeQueue(void);		// Return the first node in the queue
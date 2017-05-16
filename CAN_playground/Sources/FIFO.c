#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "FIFO.h"

// Global variables to store the FIFO pHead & pTail pointers
link pHead;	// Pointer to head node
link pTail;	// Pointer to tail node

// Initializes pHead and pTail to NULL
void InitQueue(void) 
{
	pHead = NULL;
	pTail = NULL;
}

// Return true if queue is empty
int IsQueueEmpty(void)
{
	return(pHead == NULL);
}

// Add a node to the queue
void AddToQueue(link pn)
{
	// Make this the head node if queue is empty
	if(IsQueueEmpty()) 
	{
		pHead = pn;
	}

	// If queue isn't empty, make the current tail point to the new tail
	else
	{
		pTail->pNext = pn;
	}

	// Make the link of the new node equal to NULL (in case it isn't already)
	pn->pNext = NULL;

	// Make the tail point to the new node
	pTail = pn;
}

// Return a pointer to the node at the head of the queue
link DeQueue(void)
{
	link pTemp;

	// If queue is empty return NULL
	if(IsQueueEmpty()) return NULL;

	pTemp = pHead;			// Copy the first node to a temporary location
	pHead = pHead->pNext;	// Update the head node to be the 2nd node in the queue

	return (pTemp);			// Return the old first node
}
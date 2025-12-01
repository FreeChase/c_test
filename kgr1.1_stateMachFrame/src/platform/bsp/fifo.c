#include "fifo.h"

typedef struct
{
	char front;
	char rear;
	char size;
	char capacity;
	char data[128];
}QueueZS;

QueueZS gZsQueue;

unsigned char initQueue(unsigned char capVal)
{
	gZsQueue.front = 0;
	gZsQueue.rear = -1;
	gZsQueue.size = 0;
	gZsQueue.capacity = capVal;

	return 0;
}

BOOL isEmpty()
{
	return (gZsQueue.size == 0);
}

BOOL isFull()
{
	return (gZsQueue.size == gZsQueue.capacity);
}

BOOL enqueue(unsigned char enVal)
{
	if(isFull())
	{
		return FALSE;
	}

	gZsQueue.rear = (gZsQueue.rear+ 1)% gZsQueue.capacity;
	gZsQueue.data[gZsQueue.rear] = enVal;
	gZsQueue.size++;

	return TRUE;
}

BOOL dequeue(char * enVal)
{
	if(isEmpty())
	{
		*enVal = 0;
		return FALSE;
	}

	*enVal = gZsQueue.data[gZsQueue.front];
	gZsQueue.front = (gZsQueue.front+1)%gZsQueue.capacity;

	gZsQueue.size--;

	return TRUE;
}

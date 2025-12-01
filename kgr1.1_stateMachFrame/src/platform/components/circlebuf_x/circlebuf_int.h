#ifndef _CIRCLEBUF_INT_H_
#define _CIRCLEBUF_INT_H_


//#include "dxdef.h"
//#include "dxos.h"

//#define USE_OS 0

#ifdef USE_OS
    // #include <FreeRTOS.h>
    // #include <semphr.h>
#define X_MUTEX dx_handle

#endif

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    int *buffer;
    int size;
    int head;
    int tail;
#ifdef USE_OS
    X_MUTEX readMutex;  // ¶Á»¥³âËø
    X_MUTEX writeMutex; // Ð´»¥³âËø
#endif
} CircularBuffer_Int;


void initializeBufferInt(CircularBuffer_Int * cBuffer, int * pbuf, int size);
int writeBufferIntMutex(CircularBuffer_Int *cBuffer, int data);
int writeBufferIntFromISR(CircularBuffer_Int *cBuffer, int data);
int writeBufferIntMultipleMutex(CircularBuffer_Int *cBuffer, int *data, int length);
int readBufferIntMutex(CircularBuffer_Int *cBuffer, int *data);
int readBufferIntMultipleMutex(CircularBuffer_Int *cBuffer, int *data, int length);
int readBufferIntMultipleAndClear(CircularBuffer_Int *cBuffer, int *data, int length);



int writeBufferIntNoMutex(CircularBuffer_Int *cBuffer, int data);
int readBufferIntNoMutex(CircularBuffer_Int *cBuffer, int *data);
int readBufferIntMultipleNoMutex(CircularBuffer_Int *cBuffer, int *data, int length);
int writeBufferIntMultipleNoMutex(CircularBuffer_Int *cBuffer, int *data, int length);
int readBufferIntMultipleAndClearNoMutex(CircularBuffer_Int *cBuffer, int *data, int length);
int getIntRemainingCountNoMutex(CircularBuffer_Int *cBuffer);
int getIntWritableCapacityNoMutex(CircularBuffer_Int *cBuffer);
void clearIntBufferNoMutex(CircularBuffer_Int *cBuffer);
#ifdef __cplusplus
}
#endif


#endif

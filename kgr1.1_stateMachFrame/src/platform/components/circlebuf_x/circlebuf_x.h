#ifndef _CIRCLEBUF_X_H_
#define _CIRCLEBUF_X_H_


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
    char *buffer;
    int size;
    int head;
    int tail;
#ifdef USE_OS
    X_MUTEX readMutex;  // ¶Á»¥³âËø
    X_MUTEX writeMutex; // Ð´»¥³âËø
#endif
} CircularBuffer;


void initializeBuffer(CircularBuffer * cBuffer, char * pbuf, int size);
int writeBufferMutex(CircularBuffer *cBuffer, char data);
int writeBufferFromISR(CircularBuffer *cBuffer, char data);
int writeBufferMultipleMutex(CircularBuffer *cBuffer, char *data, int length);
int readBufferMutex(CircularBuffer *cBuffer, char *data);
int readBufferMultipleMutex(CircularBuffer *cBuffer, char *data, int length);
int readBufferMultipleAndClear(CircularBuffer *cBuffer, char *data, int length);
int getRemainingCount(CircularBuffer *cBuffer);
void clearBuffer(CircularBuffer *cBuffer);


int writeBufferNoMutex(CircularBuffer *cBuffer, char data);
int readBufferNoMutex(CircularBuffer *cBuffer, char *data);
int readBufferMultipleNoMutex(CircularBuffer *cBuffer, char *data, int length);
int writeBufferMultipleNoMutex(CircularBuffer *cBuffer, char *data, int length);
int readBufferMultipleAndClearNoMutex(CircularBuffer *cBuffer, char *data, int length);
int getRemainingCountNoMutex(CircularBuffer *cBuffer);
int getWritableCapacityNoMutex(CircularBuffer *cBuffer);
void clearBufferNoMutex(CircularBuffer *cBuffer);
#ifdef __cplusplus
}
#endif


#endif

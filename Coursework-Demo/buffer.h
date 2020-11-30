#ifndef _BUFFER_GUARD
#define _BUFFER_GUARD

#include "util.h"
#include "math.h"

typedef struct Buffer {
    float* items;
    int length;
    Stats stats;
} Buffer;

Buffer
getBuffer(int size) // retrieve new buffer with malloc-ed memory space
{
    float* memSpace = (float*) malloc(size * sizeof(float));
    Buffer buffer = {memSpace, size, };
    return buffer;
}

void
freeBuffer(Buffer buffer) // little abstraction function to act as buffer destructor
{
    free(buffer.items);
}

void
swapBufferMemory(Buffer *first, Buffer *second) // swap memspaces between buffers
{
    float* firstItems = first->items; // swap input buffer and output buffer item pointers
    first->items = second->items;
    second->items = firstItems;
    
    int firstLength = first->length; // swap lengths to iterate correctly
    first->length = second->length;
    second->length = firstLength;
}

void // perform aggregation into groupSize (4 in the spec) 
aggregateBuffer(Buffer bufferIn, Buffer bufferOut, int groupSize)
{
    int requiredGroups = ceil((float)bufferIn.length/groupSize); // number of groups
    int finalGroupSize = bufferIn.length % groupSize; // work out length of final group if bufferIn not of length that divides nicely

    if(requiredGroups > bufferOut.length) // error check
    {
        putFloat((float)bufferIn.length/groupSize); printf(" length out buffer required, %i provided\n", bufferOut.length);
        return;
    }

    int g; // for group number
    float *inputPtr = bufferIn.items; // cursor for full buffer
    float *outputPtr = bufferOut.items; // cursor for output buffer
    for(g = 0; g < requiredGroups; g++)
    {        
        int length = groupSize; // length of this group's size
        if(g == requiredGroups - 1 && finalGroupSize != 0) length = finalGroupSize; // shorten if necessary
        
        *outputPtr = calculateMean(inputPtr, length); // SET OUTPUT VALUE
        
        inputPtr += length; // increment both cursors
        outputPtr++;
    }
}

void
calculateBufferEMA(Buffer inBuffer, Buffer outBuffer, float smooth)
{
    if(smooth < 0 || smooth > 1) // VALIDATE SMOOTH VALUE
    {
        printf("Smoothing value of %f not valid, between 0 and 1 required", smooth);
        return NULL;
    }

    if(inBuffer.length > outBuffer.length) // OUTPUT BUFFER LONG ENOUGH
    {
        printf("Out buffer not big enough to hold items");
        return NULL;
    }

    int i;
    float *inputPtr = inBuffer.items; // cursor for full buffer
    float *outputPtr = outBuffer.items; // cursor for output buffer

    float emaLast;
    for(i = 0; i < inBuffer.length; i++)
    {
        if(i == 0)
        {
            *outputPtr = *inputPtr; //first ema is first value
        }
        else
        {
            *outputPtr = calculateEMA(*inputPtr, emaLast, smooth);
        }
        emaLast = *outputPtr; // keep this ema for next round

        inputPtr++;
        outputPtr++;
    }
}

void 
clearBuffer(Buffer buffer)
{
    int length = buffer.length;
    if(length > 0)
    {
        int i;
        float *bufferPtr = buffer.items;
        for(i = 0; i < length; i++)
        {
            *bufferPtr = 0.0;
            bufferPtr++;
        }
    }
}

void 
printBuffer(Buffer buffer)
{
    putchar('[');

    int length = buffer.length;
    if(length > 0)
    {
        int i;
        float *ptr = buffer.items;
        for(i = 0; i < length; i++)
        {
            if(i > 0) printf(", ");

            putFloat(*ptr);
            ptr++;
        }
    }    

    putchar(']');
}

#endif

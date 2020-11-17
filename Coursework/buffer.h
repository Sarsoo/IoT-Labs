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
getBuffer(int size)
{
    float* memSpace = (float*) malloc(size * sizeof(float));
    Buffer buffer = {memSpace, size, };
    return buffer;
}

void 
aggregateBuffer(Buffer bufferIn, Buffer bufferOut, int groupSize)
{
    int requiredGroups = ceil((float)bufferIn.length/groupSize); // number of groups
    int finalGroupSize = (bufferIn.length % groupSize) * groupSize;

    if(requiredGroups > bufferOut.length) // error
    {
        putFloat((float)bufferIn.length/groupSize);
        printf(" length out buffer required, %i provided\n", bufferOut.length);
        return;
    }

    int g; // for group number
    float *inputPtr = bufferIn.items; // cursor for full buffer
    float *outputPtr = bufferOut.items; // cursor for output buffer
    for(g = 0; g < requiredGroups; g++)
    {        
        int length = groupSize; // length of this group's size
        if(g == requiredGroups - 1 && finalGroupSize != 0) length = finalGroupSize; // shorten if necessary
        
        *outputPtr = calculateMean(inputPtr, length); // SET
        
        inputPtr += length; // increment both
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

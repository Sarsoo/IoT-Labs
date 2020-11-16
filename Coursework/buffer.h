#ifndef _BUFFER_GUARD
#define _BUFFER_GUARD

#include "util.h"
#include "math.h"

float*
getBuffer(int size)
{
    return malloc(size * sizeof(float));
}

void 
aggregateBuffer(float *bufferIn, int lengthIn, float *bufferOut, int lengthOut, int groupSize)
{
    int requiredGroups = ceil((float)lengthIn/groupSize); // number of groups
    int finalGroupSize = (lengthIn % groupSize) * groupSize;

    if(requiredGroups > lengthOut) // error
    {
        putFloat((float)lengthIn/groupSize);
        printf(" length out buffer required, %i provided\n", lengthOut);
        return;
    }

    int g; // for group number
    float *inputPtr = bufferIn; // cursor for full buffer
    float *outputPtr = bufferOut; // cursor for output buffer
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
clearBuffer(float *buffer, int length)
{
    if(length > 0)
    {
        int i;
        float *bufferPtr = buffer;
        for(i = 0; i < length; i++)
        {
            *bufferPtr = 0.0;
            bufferPtr++;
        }
    }
}

void 
printBuffer(float *buffer, int length)
{
    putchar('[');

    if(length > 0)
    {
        int i;
        float *ptr = buffer;
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

#ifndef _BUFFER_GUARD
#define _BUFFER_GUARD

#include "util.h"

void 
aggregateBuffer(float *bufferIn, int lengthIn, float *bufferOut, int lengthOut, int groupSize)
{
    int requiredGroups = ceil((float)lengthIn/groupSize);
    int finalGroupSize = (lengthIn % groupSize) * groupSize;

    if(requiredGroups > lengthOut)
    {
        putFloat((float)lengthIn/groupSize);
        printf(" length out buffer required, %i provided\n", lengthOut);
        return;
    }

    int g;// for group number
    float *inputPtr = bufferIn;
    float *outputPtr = bufferOut;
    for(g = 0; g < requiredGroups; g++)
    {        
        int length = groupSize;
        if(g == requiredGroups - 1 && finalGroupSize != 0) length = finalGroupSize;
        
        *outputPtr = calculateMean(inputPtr, length);
        
        inputPtr += length;
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

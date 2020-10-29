#ifndef _BUFFER_GUARD
#define _BUFFER_GUARD

#include "util.h"

void 
aggregateBuffer(float bufferIn[], int lengthIn, float bufferOut[], int lengthOut, int groupSize)
{
    int requiredGroups = ceil((float)lengthIn/groupSize);
    int finalGroupSize = (lengthIn % groupSize) * groupSize;

    if(requiredGroups > lengthOut)
    {
        putFloat((float)lengthIn/groupSize);
        printf(" length out buffer required, %i provided", lengthOut);
        return;
    }

    int g;// for group number
    for(g = 0; g < requiredGroups; g++)
    {
        int length = groupSize;
        if(g == requiredGroups - 1 && finalGroupSize != 0) length = finalGroupSize;
        bufferOut[g] = calculateMean(&bufferIn + (g*groupSize), length);
    }
}

void 
clearBuffer(float buffer[], int length)
{
    if(length > 0)
    {
        int i;
        for(i = 0; i < length; i++)
        {
            buffer[i] = 0.0;
        }
    }
}

void 
printBuffer(float buffer[], int length)
{
    putchar('[');

    if(length > 0)
    {
        int i;
        for(i = 0; i < length; i++)
        {
            if(i > 0) printf(", ");

            putFloat(buffer[i]);
        }
    }    

    putchar(']');
}

#endif

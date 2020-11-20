#ifndef _SAX_GUARD
#define _SAX_GUARD

#define SAX_CHAR_START 'a'

#ifndef SAX_BREAKPOINTS
    #define SAX_BREAKPOINTS 4
#endif

// Could have used a 2d array for breakpoints, index by number of breakpoints

// Since the number of boundaries is known at compile-time, save these lookup calls by
// defining as as constant 1D arrays

#if SAX_BREAKPOINTS == 3
    const float breakPoints[] = {-0.43, 0.43};
#elif SAX_BREAKPOINTS == 4
    const float breakPoints[] = {-0.67, 0, 0.67};
#elif SAX_BREAKPOINTS == 5
    const float breakPoints[] = {-0.84, -0.25, 0.25, 0.84};
#elif SAX_BREAKPOINTS == 6
    const float breakPoints[] = {-0.97, -0.43, 0, 0.43, 0.97};
#elif SAX_BREAKPOINTS == 7
    const float breakPoints[] = {-1.07, -0.57, -0.18, 0.18, 0.57, 1.07};
#elif SAX_BREAKPOINTS == 8
    const float breakPoints[] = {-1.15, -0.67, -0.32, 0, 0.32, 0.67, 1.15};
#elif SAX_BREAKPOINTS == 9
    const float breakPoints[] = {-1.22, -0.76, -0.43, -0.14, 0.14, 0.43, 0.76, 1.22};
#elif SAX_BREAKPOINTS == 10
    const float breakPoints[] = {-1.28, -0.84, -0.52, -0.25, 0, 0.25, 0.52, 0.84, 1.28};
#else
    #define SAX_BREAKPOINTS 4
    const float breakPoints[] = {-0.67, 0, 0.67};
#endif

void
normaliseBuffer(Buffer bufferIn) // z normalise buffer for SAX
{
    if(bufferIn.stats.std == 0) // error check, don't divide by 0
    {
        printf("Standard deviation of zero, unable to normalise\n");
        return;
    }

    int i;
    float *inputPtr = bufferIn.items; // cursor
    for(i = 0; i < bufferIn.length; i++)
    {
        *inputPtr = (*inputPtr - bufferIn.stats.mean) / bufferIn.stats.std;
        
        inputPtr++;
    }
}

char
valueToSAXChar(float inputValue)
{
    int i;
    for(i = 0; i < SAX_BREAKPOINTS; i++)
    {
        if(i == 0) // first iter, is less than first breakpoint
        {
            if(inputValue < breakPoints[i]) return SAX_CHAR_START + i;
        }
        else if(i == SAX_BREAKPOINTS - 1) // last iter, is more than last breakpoint
        {
            if(breakPoints[i - 1] < inputValue) return SAX_CHAR_START + i;
        }
        else // in between check interval of two breakpoints
        {
            if((breakPoints[i - 1] < inputValue) && (inputValue  < breakPoints[i])) return SAX_CHAR_START + i;
        }
    }
    return '0';
}

char* // map buffer of normalised floats into SAX chars
stringifyBuffer(Buffer bufferIn)
{
    char* outputString = (char*) malloc((bufferIn.length + 1) * sizeof(char)); // +1 for null terminator
    
    int i;
    for(i = 0; i < bufferIn.length; i++)
    {
        outputString[i] = valueToSAXChar(bufferIn.items[i]);
    }
    
    outputString[bufferIn.length] = '\0'; // add null terminator
    return outputString;
}

#endif

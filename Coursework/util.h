#ifndef _UTIL_GUARD
#define _UTIL_GUARD

typedef unsigned short USHORT;

//print a unsigned short (as returned from rand) picewise char by char
void
putShort(USHORT in) 
{
    // recursively shift each digit of the int to units from most to least significant
    if (in >= 10)
    {
        putShort(in / 10);
    }
    // isolate unit digit from each number by modulo and add '0' char to turn integer into corresponding ascii char
    putchar((in % 10) + '0');
}

void
putFloat(float in)
{
    if(in < 0) 
    {
        putchar('-'); // print negative sign if required
        in = -in;
    }	
	
    USHORT integerComponent = (USHORT) in; // truncate float to integer
    float fractionComponent =  (in - integerComponent) * 1000; // take fraction only and promote to integer
    if (fractionComponent - (USHORT)fractionComponent >= 0.5) fractionComponent++; // round

    putShort(integerComponent);
    putchar('.');
    putShort((USHORT) fractionComponent);
}

#endif

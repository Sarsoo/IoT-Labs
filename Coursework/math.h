#ifndef _MATH_GUARD
#define _MATH_GUARD

typedef struct Stats {
    float mean;
    float std;
} Stats;

int
ceil(float in) // self-implement ceil func, no math.h
{
    int num = (int) in;
    if(in - num > 0) num++;
    return num;
}

float
sqrt(float in) // self-implement sqrt func, no math.h
{
    float sqrt = in/2;
    float temp = 0;

    while(sqrt != temp)
    {
        temp = sqrt;
        sqrt = (in/temp + temp) / 2;
    }
    return sqrt;
}

float
calculateMean(float buffer[], int length)
{
    if(length <= 0)
    {
        printf("%i items is not valid length\n", length);
        return 0;
    }
    
    /* SUM */
    float sum = 0;
    int i;
    for(i = 0; i < length; i++)
    {
        sum += buffer[i];
    }
    
    return sum / length; // DIVIDE ON RETURN
}

Stats
calculateStdDev(float buffer[], int length)
{
    Stats stats;
    if(length <= 0)
    {
        printf("%i items is not valid length\n", length);
        return stats;
    }

    stats.mean = calculateMean(buffer, length);

    float sum = 0;
    int i;
    for(i = 0; i < length; i++)
    {
        float diffFromMean = buffer[i] - stats.mean; // (xi - mu)
        sum += diffFromMean*diffFromMean; // Sum(diff squared)
    }
    
    stats.std = sqrt(sum/length);

    return stats;
}

#endif

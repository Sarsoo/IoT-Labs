#define READING_INTERVAL 3 //in Hz
#define BUFFER_SIZE 5

#define SD_THRESHOLD 3
#define AGGREGATION_GROUP_SIZE 4

#include "contiki.h"
#include "dev/light-sensor.h"

#include <stdio.h> /* For printf() */

#include "util.h" // for print methods
#include "math.h"
#include "buffer.h"

// get float from light sensor including T function
float
getLight(void)
{
    int lightData = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);

    float V_sensor = 1.5 * lightData / 4096;
    float I = V_sensor/1e5;
    float light = 0.625 * 1e6 * I * 1000;
    return light;
}

// Process final buffer following aggregation
void
handleFinalBuffer(float buffer[], int length)
{
    printf("Final buffer output: ");
    printBuffer(buffer, length);putchar('\n');putchar('\n');
}

// Buffer filled with readings, process and aggregate
void
handleBufferRotation(float buffer[], int length)
{
    printf("Buffer full, aggregating\n\n");
    float aggregated[BUFFER_SIZE];

    Stats sd = calculateStdDev(buffer, length);
    if(sd.std > SD_THRESHOLD)
    {// buffer length by 4
        printf("Significant STD: ");
        putFloat(sd.std);
        printf(", compressing buffer\n");

        float outBuffer[BUFFER_SIZE];
        int outLength = ceil((float)length/AGGREGATION_GROUP_SIZE);
        aggregateBuffer(buffer, length, outBuffer, outLength, AGGREGATION_GROUP_SIZE);

        handleFinalBuffer(outBuffer, outLength);
    }else
    {// buffer length to 1
        printf("Insignificant STD: ");
        putFloat(sd.std);
        printf(", squashing buffer\n");

        handleFinalBuffer(&sd.mean, 1);
    }
    clearBuffer(buffer, length);
}

float buffer[BUFFER_SIZE];

/*---------------------------------------------------------------------------*/
PROCESS(aggregator_process, "Aggregator process");
AUTOSTART_PROCESSES(&aggregator_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(aggregator_process, ev, data)
{
    /*INIT*/
    static struct etimer timer;
    PROCESS_BEGIN();

    etimer_set(&timer, CLOCK_SECOND/READING_INTERVAL);
    SENSORS_ACTIVATE(light_sensor);
    
    clearBuffer(buffer, BUFFER_SIZE);
    printBuffer(buffer, BUFFER_SIZE);putchar('\n');putchar('\n');
    /*END INIT*/
    
    static int counter = 0;
    while(1)
    {
        PROCESS_WAIT_EVENT_UNTIL(ev=PROCESS_EVENT_TIMER);
        
        float light_lx = getLight();

        buffer[counter] = light_lx;
        printBuffer(buffer, BUFFER_SIZE);putchar('\n');
        
        counter++;
        if(counter == BUFFER_SIZE)
        {
            handleBufferRotation(buffer, BUFFER_SIZE);
            counter = 0;
        }

        etimer_reset(&timer);
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/

#define READING_INTERVAL 3 //in Hz
#define BUFFER_SIZE 12 // length of buffer to populate

#define SD_THRESHOLD 0 // whether to aggregate or flatten
#define AGGREGATION_GROUP_SIZE 4 // group size to aggregate (4 in spec)

#include "contiki.h"
#include "dev/light-sensor.h"

#include <stdio.h> /* For printf() */

#include "util.h" // for print methods
#include "math.h"
#include "buffer.h"

//text to uncomment and make the sim reload new source

// get float from light sensor including transfer function
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
handleFinalBuffer(float *buffer, int length)
{
    printf("Final buffer output: ");
    printBuffer(buffer, length);putchar('\n');putchar('\n');
}

// Buffer filled with readings, process and aggregate
void
handleBufferRotation(float *buffer, int length)
{
    printf("Buffer full, aggregating\n\n");

    Stats sd = calculateStdDev(buffer, length); // GET BUFFER STATISTICS
    if(sd.std > SD_THRESHOLD)
    {// buffer length by 4
        printf("Significant STD: ");
        putFloat(sd.std);
        printf(", compressing buffer\n");

        float outBuffer[BUFFER_SIZE]; // CREATE OUTPUT BUFFER
        clearBuffer(&outBuffer, BUFFER_SIZE);
        
        int outLength = ceil((float)length/AGGREGATION_GROUP_SIZE); // CALCULATE NUMBER OF OUTPUT ELEMENTS
        aggregateBuffer(buffer, length, &outBuffer, outLength, AGGREGATION_GROUP_SIZE);
    
        handleFinalBuffer(&outBuffer, outLength); // PASS FINAL BUFFER
    }else
    {// buffer length to 1
        printf("Insignificant STD: ");
        putFloat(sd.std);
        printf(", squashing buffer\n");

        handleFinalBuffer(&sd.mean, 1); // PASS FINAL BUFFER
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
    
    clearBuffer(&buffer, BUFFER_SIZE);
    printBuffer(&buffer, BUFFER_SIZE);putchar('\n');putchar('\n');
    /*END INIT*/
    
    static int counter = 0;
    while(1)
    {
        PROCESS_WAIT_EVENT_UNTIL(ev=PROCESS_EVENT_TIMER);
        
        float light_lx = getLight(); // GET

        buffer[counter] = light_lx; // STORE
        printBuffer(&buffer, BUFFER_SIZE);putchar('\n'); // DISPLAY
        
        counter++;
        if(counter == BUFFER_SIZE) // CHECK WHETHER FULL
        {
            handleBufferRotation(buffer, BUFFER_SIZE); // PASS OUT IF FULL
            counter = 0;
        }

        etimer_reset(&timer);
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/

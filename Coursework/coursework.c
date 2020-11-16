#define READING_INTERVAL 1 //in Hz
#define BUFFER_SIZE 12 // length of buffer to populate

#define SD_THRESHOLD 3 // whether to aggregate or flatten
#define AGGREGATION_GROUP_SIZE 4 // group size to aggregate (4 in spec)

#include "contiki.h"

#include <stdio.h> /* For printf() */

#include "io.h"
#include "util.h" // for print methods
#include "math.h"
#include "buffer.h"

// text to uncomment and make the sim reload new source

static process_event_t event_buffer_full;

/*---------------------------------------------------------------------------*/
PROCESS(sensing_process, "Sensing process");
PROCESS(aggregator_process, "Aggregator process");

AUTOSTART_PROCESSES(&sensing_process, &aggregator_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sensing_process, ev, data)
{
    /*INIT*/
    static struct etimer timer;
    PROCESS_BEGIN();
    event_buffer_full = process_alloc_event();
    etimer_set(&timer, CLOCK_SECOND/READING_INTERVAL);
    
    SENSORS_ACTIVATE(light_sensor);
    leds_off(LEDS_ALL);
    
    static float* buffer;
    buffer = getBuffer(BUFFER_SIZE);
    clearBuffer(buffer, BUFFER_SIZE);
    printBuffer(buffer, BUFFER_SIZE);putchar('\n');putchar('\n');
    /*END INIT*/
    
    static int counter = 0;
    while(1)
    {
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
        leds_off(LEDS_RED);
        
        float light_lx = getLight(); // GET

        buffer[counter] = light_lx; // STORE
        
        printf("%2i/%i: ", counter + 1, BUFFER_SIZE);putFloat(light_lx);putchar('\n'); // DISPLAY VALUE
        //printBuffer(buffer, BUFFER_SIZE);putchar('\n'); // DISPLAY CURRENT BUFFER
        
        counter++;
        if(counter == BUFFER_SIZE) // CHECK WHETHER FULL
        {
            process_post(&aggregator_process, event_buffer_full, &buffer);
            counter = 0;
            buffer = getBuffer(BUFFER_SIZE);
        }

        etimer_reset(&timer);
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(aggregator_process, ev, data)
{
    PROCESS_BEGIN();
    
    while (1)
    {
        PROCESS_WAIT_EVENT_UNTIL(ev == event_buffer_full);
        leds_on(LEDS_RED);

        float *fullBuffer = *(float **)data;
        handleBufferRotation(fullBuffer, BUFFER_SIZE);
        free(fullBuffer);
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
// Buffer filled with readings, process and aggregate
void
handleBufferRotation(float *buffer, int length)
{
    printf("Buffer full, aggregating\n\n");

    Stats sd = calculateStdDev(buffer, length); // GET BUFFER STATISTICS
    if(sd.std > SD_THRESHOLD)
    {// buffer length by 4
        printf("Significant STD: ");putFloat(sd.std);printf(", compressing buffer\n");

        float *outBuffer = getBuffer(length); // CREATE OUTPUT BUFFER
        clearBuffer(outBuffer, length);
        
        int outLength = ceil((float)length/AGGREGATION_GROUP_SIZE); // CALCULATE NUMBER OF OUTPUT ELEMENTS
        aggregateBuffer(buffer, length, outBuffer, outLength, AGGREGATION_GROUP_SIZE);

        handleFinalBuffer(outBuffer, outLength); // PASS FINAL BUFFER
        free(outBuffer); // RELEASE
    }else
    {// buffer length to 1
        printf("Insignificant STD: ");putFloat(sd.std);printf(", squashing buffer\n");

        handleFinalBuffer(&sd.mean, 1); // PASS FINAL BUFFER
    }
}

// Process final buffer following aggregation
void
handleFinalBuffer(float *buffer, int length)
{
    printf("Final buffer output: ");
    printBuffer(buffer, length);putchar('\n');putchar('\n');
}
/*---------------------------------------------------------------------------*/

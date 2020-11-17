#define READING_INTERVAL 3 //in Hz
#define BUFFER_SIZE 9 // length of buffer to populate

#define SD_THRESHOLD 300 // whether to aggregate or flatten
#define AGGREGATION_GROUP_SIZE 3 // group size to aggregate (4 in spec)

#include "contiki.h"

#include <stdio.h> /* For printf() */

#include "io.h"
#include "util.h" // for print methods
#include "math.h"
#include "buffer.h"

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
    
    static Buffer buffer;
    buffer = getBuffer(BUFFER_SIZE);
    clearBuffer(buffer);
    printBuffer(buffer);putchar('\n');putchar('\n');
    /*END INIT*/
    
    static int counter = 0;
    while(1)
    {
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
        leds_off(LEDS_RED);
        
        float light_lx = getLight(); // GET

        buffer.items[counter] = light_lx; // STORE
        
        printf("%2i/%i: ", counter + 1, buffer.length);putFloat(light_lx);putchar('\n'); // DISPLAY VALUE
        //printBuffer(buffer, BUFFER_SIZE);putchar('\n'); // DISPLAY CURRENT BUFFER
        
        counter++;
        if(counter == buffer.length) // CHECK WHETHER FULL
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

        Buffer fullBuffer = *(Buffer *)data;
        /*********************/
        handleBufferRotation(fullBuffer);
        free(fullBuffer.items);
        /*********************/
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
// Buffer filled with readings, process and aggregate
void
handleBufferRotation(Buffer inBuffer)
{
    printf("Buffer full, aggregating\n\n");
    
    Buffer outBuffer; // OUTPUT BUFFER HOLDER
    // above pointer is assigned a buffer in either of the below cases

    Stats sd = calculateStdDev(inBuffer.items, inBuffer.length); // GET BUFFER STATISTICS
    if(sd.std > SD_THRESHOLD)
    {// buffer length by 4
        printf("Significant STD: ");putFloat(sd.std);printf(", compressing buffer\n");
        
        int outLength = ceil((float)inBuffer.length/AGGREGATION_GROUP_SIZE); // CALCULATE NUMBER OF OUTPUT ELEMENTS
        outBuffer = getBuffer(outLength); // CREATE OUTPUT BUFFER
    
        aggregateBuffer(inBuffer, outBuffer, AGGREGATION_GROUP_SIZE);

    }else
    {// buffer length to 1
        printf("Insignificant STD: ");putFloat(sd.std);printf(", squashing buffer\n");
        
        outBuffer = getBuffer(1); // CREATE OUTPUT BUFFER
        outBuffer.items[0] = sd.mean;
    }
    outBuffer.stats = sd; // final compressed buffer has pointer to stats for uncompressed data in case of further interest
    
    /*********************/
    handleFinalBuffer(outBuffer); // PASS FINAL BUFFER
    free(outBuffer.items); // RELEASE ITEMS
    /*********************/
}

// Process final buffer following aggregation
void
handleFinalBuffer(Buffer buffer)
{
    printf("Final buffer output: ");
    printBuffer(buffer);putchar('\n');
    printf("Mean: ");putFloat(buffer.stats.mean);putchar('\n');
    printf("Std Dev: ");putFloat(buffer.stats.std);putchar('\n');putchar('\n');
}
/*---------------------------------------------------------------------------*/

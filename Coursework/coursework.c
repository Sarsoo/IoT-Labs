#define READING_INTERVAL 2 //in Hz
#define BUFFER_SIZE 12 // length of buffer to populate

// below thresholds are calibrated for the cooja slider
// they are likely not suitable for using on the mote
#define SD_THRESHOLD_SOME 400 // some activity, 4-to-1 above, 12-to-1 below
#define SD_THRESHOLD_LOTS 1000 // lots of activity, don't aggregate

#define AGGREGATION_GROUP_SIZE 4 // group size to aggregate (4 in spec)

#define INITIAL_STATE true // whether begins running or not

#define SAX // use sax aggregation and transform instead of simple average aggregation
#define SAX_BREAKPOINTS 4 // number of characters to be used

#include "contiki.h"

#include <stdio.h> /* For printf() */
#include <stdbool.h>

#include "io.h"
#include "util.h" // for print methods
#include "math.h"
#include "buffer.h"
#include "sax.h"

static process_event_t event_buffer_full;

/*---------------------------------------------------------------------------*/
PROCESS(sensing_process, "Sensing process"); // collect data
PROCESS(aggregator_process, "Aggregator process"); // receive full data buffers for processing

AUTOSTART_PROCESSES(&sensing_process, &aggregator_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sensing_process, ev, data)
{
    /*INIT*/
    PROCESS_BEGIN();
    
    static bool isRunning = INITIAL_STATE;
    static struct etimer timer;
    if(isRunning) etimer_set(&timer, CLOCK_SECOND/READING_INTERVAL); // start timer if running
    
    event_buffer_full = process_alloc_event(); // event for passing full buffers away for processing
    initIO();
    
    static Buffer buffer;
    buffer = getBuffer(BUFFER_SIZE);
    /*END INIT*/
    
    static int counter = 0;
    while(1)
    {
        PROCESS_WAIT_EVENT();
        
        if (ev == PROCESS_EVENT_TIMER){
            leds_off(LEDS_RED);
            
            float light_lx = getLight(); // GET

            buffer.items[counter] = light_lx; // STORE
            
            printf("%2i/%i: ", counter + 1, buffer.length);putFloat(light_lx);putchar('\n'); // DISPLAY CURRENT VALUE
            //printBuffer(buffer);putchar('\n'); // DISPLAY CURRENT BUFFER
            
            counter++;
            if(counter == buffer.length) // CHECK WHETHER FULL
            {                
                process_post(&aggregator_process, event_buffer_full, &buffer); // pass buffer to processing thread
                counter = 0;
                buffer = getBuffer(BUFFER_SIZE); // get new buffer for next data, no freeing in this thread
            }
            
            etimer_reset(&timer);
        }
        /* BUTTON CLICKED */
        else if (ev == sensors_event && data == &button_sensor)
        {
            isRunning = !isRunning;
            if (isRunning == true)
            {
                printf("Starting...\n");
                etimer_set(&timer, CLOCK_SECOND/READING_INTERVAL);
            }
            else
            {
                printf("Stopping, clearing buffer...\n");
                etimer_stop(&timer);
                counter = 0; // just reset counter, used as index on buffer items, will overwrite
            }
        }
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
#ifdef SAX
        handleSAXBufferRotation(&fullBuffer);
#else
        handleSimpleBufferRotation(&fullBuffer); // pass by reference, edited if lots of activity
#endif
        freeBuffer(fullBuffer);
        /*********************/
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
// Buffer filled with readings, process and aggregate
void
handleSimpleBufferRotation(Buffer *inBufferPtr)
{
    printf("Buffer full, aggregating\n\n");
    
    Buffer inBuffer = *inBufferPtr;
    Buffer outBuffer; // OUTPUT BUFFER HOLDER
    // above pointer is assigned a buffer in either of the below cases

    Stats sd = calculateStdDev(inBuffer.items, inBuffer.length); // GET BUFFER STATISTICS
    
    printf("B = ");printBuffer(inBuffer);putchar('\n');
    printf("StdDev = ");putFloat(sd.std);putchar('\n');
    printf("Aggregation = ");
    
    /* LOTS OF ACTIVITY - LEAVE */
    if(sd.std > SD_THRESHOLD_LOTS)
    {
        //printf("Lots of activity, std. dev.: ");putFloat(sd.std);printf(", leaving as-is\n");
        puts("None");
        
        outBuffer = getBuffer(1); // get a dummy buffer, will swap items for efficiency
        
        swapBufferMemory(inBufferPtr, &outBuffer); // ensures both are freed but no need to copy items
        
    }
    /* SOME ACTIVITY - AGGREGATE */
    else if(sd.std > SD_THRESHOLD_SOME)
    {
        //printf("Some activity, std. dev.: ");putFloat(sd.std);printf(", compressing buffer\n");
        puts("4-into-1");
        
        int outLength = ceil((float)inBuffer.length/AGGREGATION_GROUP_SIZE); // CALCULATE NUMBER OF OUTPUT ELEMENTS
        outBuffer = getBuffer(outLength); // CREATE OUTPUT BUFFER
    
        aggregateBuffer(inBuffer, outBuffer, AGGREGATION_GROUP_SIZE);

    }
    /* NO ACTIVITY - FLATTEN */
    else
    {
        //printf("Insignificant std. dev.: ");putFloat(sd.std);printf(", squashing buffer\n");
        puts("12-into-1");
        
        outBuffer = getBuffer(1); // CREATE OUTPUT BUFFER
        
        outBuffer.items[0] = sd.mean;
    }
    outBuffer.stats = sd; // final compressed buffer has stats for uncompressed data in case of further interest
    
    /*********************/
    handleFinalBuffer(outBuffer); // PASS FINAL BUFFER
    freeBuffer(outBuffer); // RELEASE ITEMS
    /*********************/
}

void
handleSAXBufferRotation(Buffer *inBufferPtr)
{
    printf("Buffer full, SAX-ing\n\n");
    
    Buffer inBuffer = *inBufferPtr;
    Buffer outBuffer; // OUTPUT BUFFER HOLDER
    // above pointer is assigned a buffer in either of the below cases

    int outLength = ceil((float)inBuffer.length/AGGREGATION_GROUP_SIZE); // CALCULATE NUMBER OF OUTPUT ELEMENTS
    outBuffer = getBuffer(outLength); // CREATE OUTPUT BUFFER
    
    inBuffer.stats = calculateStdDev(inBuffer.items, inBuffer.length); // GET BUFFER STATISTICS
    outBuffer.stats = inBuffer.stats;
    
    normaliseBuffer(inBuffer); // Z NORMALISATION
    aggregateBuffer(inBuffer, outBuffer, AGGREGATION_GROUP_SIZE); // PAA
    
    /*********************/
    handleFinalBuffer(outBuffer); // PASS FINAL BUFFER
    freeBuffer(outBuffer); // RELEASE ITEMS
    /*********************/
}

// Process final buffer following aggregation
void
handleFinalBuffer(Buffer buffer)
{
    printf("X: ");printBuffer(buffer);putchar('\n');
#ifdef SAX
    printf("Mean: ");putFloat(buffer.stats.mean);putchar('\n');
    printf("Std Dev: ");putFloat(buffer.stats.std);putchar('\n');putchar('\n');    

    char* saxString = stringifyBuffer(buffer);
    printf("SAX: %s\n", saxString);
    
    free(saxString);
#endif
    putchar('\n');
}
/*---------------------------------------------------------------------------*/

#include "contiki.h"
#include <stdio.h> /* For printf() */

#include "dev/sht11-sensor.h"
#include "dev/light-sensor.h"

#define MAX_VALUES  4 // Number of values used in averaging process

static process_event_t event_data_ready; // Application specific event value

/*---------------------------------------------------------------------------*/
/* We declare the two processes */
PROCESS(temp_process, "Temperature process");
PROCESS(print_process, "Print process");

/* We require the processes to be started automatically */
AUTOSTART_PROCESSES(&temp_process, &print_process);
/*---------------------------------------------------------------------------*/
/* Implementation of the first process */
PROCESS_THREAD(temp_process, ev, data)
{
    /* Variables are declared static to ensure their values are kept */
    /* between kernel calls. */
    static struct etimer timer;
    static int count = 0;

    // Any process must start with this.
    PROCESS_BEGIN();

    // Allocate the required event
    event_data_ready = process_alloc_event();

    // Initialise the temperature sensor
    SENSORS_ACTIVATE(light_sensor);  // need this for sky-mote emulation
    SENSORS_ACTIVATE(sht11_sensor);

    // Initialise variables
    count = 0;

    // Set the etimer module to generate a periodic event
    etimer_set(&timer, CLOCK_CONF_SECOND);

    static int myData;
    while (1)
    {
        // Wait here for the timer to expire
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

        // this is temperature process
       	count++;
        //int tempVal = sht11_sensor.value(SHT11_SENSOR_TEMP_SKYSIM); // For Cooja Sim
        int tempVal = sht11_sensor.value(SHT11_SENSOR_TEMP); // For XM1000 mote
        printf("[temperature process] temp=%d\n", tempVal);

        // Check if enough samples are collected
       	if (count==MAX_VALUES)
       	{
       	  // Transfer the last read raw temperature reading for passing
       	  myData = tempVal;

       	  // Reset variables
       	  count = 0;

       	  // Post an event to the print process
       	  // and pass a pointer to the last measure as data
          printf("[temperature process] passing value %d\n", myData);
          process_post(&print_process, event_data_ready, &myData);
        }

        // Reset the timer so it will generate another event
        etimer_reset(&timer);
     }

     // Any process must end with this, even if it is never reached.
     PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/* Implementation of the second process */
PROCESS_THREAD(print_process, ev, data)
{
    PROCESS_BEGIN();

    while (1)
    {
      // Wait until we get a data_ready event
      PROCESS_WAIT_EVENT_UNTIL(ev == event_data_ready);

      // Use 'data' variable to retrieve data and then display it
      int myData = *(int *)data; // cast to an integer pointer
      printf("[print process] data ready, received value = %d\n", myData);
    }
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/

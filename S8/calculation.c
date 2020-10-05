#include "contiki.h"
#include "dev/light-sensor.h"
#include "dev/sht11-sensor.h"
#include "dev/button-sensor.h"
#include <stdio.h>  // For printf()

/*---------------------------------------------------------------------------*/
PROCESS(calculation, "Calculation");
AUTOSTART_PROCESSES(&calculation);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(calculation, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();
  etimer_set(&timer, CLOCK_CONF_SECOND);

  SENSORS_ACTIVATE(light_sensor);  // need this for temperature sensor in Simulation
  SENSORS_ACTIVATE(sht11_sensor);
  SENSORS_ACTIVATE(button_sensor); // activate button too

  while(1) 
  {
    PROCESS_WAIT_EVENT(); // wait for an event

    if (ev==PROCESS_EVENT_TIMER)
    {
      int lightData = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);
      printf("Reading (raw light intensity) = %d\n", lightData);

      etimer_reset(&timer);
    }
    else if (ev==sensors_event && data==&button_sensor)
    {
      // When a button is pressed, we should calculate the square root
      int lightData = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);
      printf("Reading (raw light intensity) = %d\n", lightData);

      // Do your calculation here...
      printf("The square root of the last reading = ???\n");
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

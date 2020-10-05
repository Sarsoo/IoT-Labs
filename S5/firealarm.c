#include "contiki.h"
#include "dev/light-sensor.h"
#include "dev/sht11-sensor.h"
#include "dev/button-sensor.h" // For button
#include "dev/leds.h"    // For LED
#include <stdio.h> /* For printf() */

/*---------------------------------------------------------------------------*/
PROCESS(fire_alarm_process, "Firealarm process");
AUTOSTART_PROCESSES(&fire_alarm_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(fire_alarm_process, ev, data)
{
  static struct etimer timer;
  PROCESS_BEGIN();
  etimer_set(&timer, CLOCK_CONF_SECOND/2);

  SENSORS_ACTIVATE(light_sensor);  // need this for temperature sensor in Simulation
  SENSORS_ACTIVATE(sht11_sensor);
  SENSORS_ACTIVATE(button_sensor); // activate button too
  leds_off(LEDS_ALL);              // turn off all LEDs

  static int counter = 0;
  while(1)
  {
    PROCESS_WAIT_EVENT(); // wait for an event

    if (ev==PROCESS_EVENT_TIMER) 
    {
      // timer event
      printf("%d\n",++counter);
      if (counter==10) leds_on(LEDS_ALL);

      etimer_reset(&timer);
    }
    else if (ev==sensors_event && data==&button_sensor) 
    {
      // button event
      counter = 0;
      leds_off(LEDS_ALL);
      printf("%d\n",counter);
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/


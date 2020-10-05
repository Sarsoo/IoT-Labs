#include "contiki.h" 
#include "random.h" 
#include "powertrace.h" 
#include "dev/leds.h"

#include <stdio.h> 

PROCESS(power, "powertrace example"); 
AUTOSTART_PROCESSES(&power); 

int d1(float f) // Integer part
{
  return((int)f);
}
unsigned int d2(float f) // Fractional part
{
  if (f>0)
    return(1000*(f-d1(f)));
  else
    return(1000*(d1(f)-f));
}


PROCESS_THREAD(power, ev, data) 
{ 
  static struct etimer et; 
  static float t;

  PROCESS_BEGIN(); 

  /* Start powertracing */ 
  int n = 1; // 1 second reporting cycle 

  powertrace_start(CLOCK_SECOND * n); 
  printf("Ticks per second: %u\n", RTIMER_SECOND); 

  while(1) 
  {
    /* Delay 2-4 seconds and then perform some tasks */ 
    t = 2*((float)random_rand()/RANDOM_RAND_MAX) + 2; 
    etimer_set(&et, CLOCK_SECOND*t); 
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et)); 

    // Put your tasks here... 

    // Task 1: Switching LED
    static int ledStatus = 0;
    switch(ledStatus) {
      case 0: leds_on(LEDS_RED);
              leds_off(LEDS_GREEN);
              leds_off(LEDS_BLUE);
              break;
      case 1: leds_off(LEDS_RED);
              leds_on(LEDS_GREEN);
              leds_off(LEDS_BLUE);
              break;
      case 2: leds_off(LEDS_RED);
              leds_off(LEDS_GREEN);
              leds_on(LEDS_BLUE);
              break;
      default:
              leds_on(LEDS_ALL);
              ledStatus = -1;
    }
    ledStatus++;

    // Task 2: Squaring a floating point
    float sq = t*t;
    printf("floor(sq)=%d\n", (int)sq);
  } 

  PROCESS_END(); 
}

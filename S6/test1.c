#include "contiki.h" 
#include "random.h" 
#include "powertrace.h" 

#include <stdio.h> 

PROCESS(power, "powertrace example"); 
AUTOSTART_PROCESSES(&power); 

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
  } 

  PROCESS_END(); 
}

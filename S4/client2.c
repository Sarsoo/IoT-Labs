#include "contiki.h"
#include "net/rime.h"
#include "dev/light-sensor.h"

#include <stdio.h>
#include <string.h>

/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "Example unicast");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/
static void
recv_uc(struct unicast_conn *c, const rimeaddr_t *from)
{
  // Print the received message
  char *dataReceived = (char *)packetbuf_dataptr();
  dataReceived[packetbuf_datalen()] = 0;
  printf("A unicast received from %d.%d\n",from->u8[0],from->u8[1]);
  printf("The message is: '%s'\n",dataReceived);
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data)
{
  PROCESS_EXITHANDLER(unicast_close(&uc);)

  PROCESS_BEGIN();

  printf("I'm a client mote, my rime addr is: %d.%d\n",
           rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[1]);
  unicast_open(&uc, 146, &unicast_callbacks);

  SENSORS_ACTIVATE(light_sensor);

  while(1)
  {
    static struct etimer et;
    rimeaddr_t addr;

    etimer_set(&et, CLOCK_SECOND*5/3);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    int lightData = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);

    char message[100];
    sprintf(message,"From %d.%d; Light (raw reading) = %d",
           rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[1], lightData);

    packetbuf_copyfrom(message, strlen(message));
    addr.u8[0] = 1;  // this is the server rime address (part 1)
    addr.u8[1] = 0;  // this is the server rime address (part 2)
    if(!rimeaddr_cmp(&addr, &rimeaddr_node_addr)) 
    {
      unicast_send(&uc, &addr);
      printf("My light reading is sent to %d.%d.\n", addr.u8[0],addr.u8[1]);
    }

  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

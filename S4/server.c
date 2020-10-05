#include "contiki.h"
#include "net/rime.h"

#include <stdio.h>

static struct unicast_conn uc;
static int    numPkts = 0;

/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "Example unicast");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/
static void
recv_uc(struct unicast_conn *c, const rimeaddr_t *from)
{
  // Record number of packets received
  numPkts++;

  // Print the received message
  char *dataReceived = (char *)packetbuf_dataptr();
  dataReceived[packetbuf_datalen()] = 0;
  printf("A unicast received from %d.%d\n",from->u8[0],from->u8[1]);
  printf("The message is:\n '%s'\n",dataReceived);

  // Reply ACK
  rimeaddr_t addr;
  addr.u8[0] = from->u8[0];
  addr.u8[1] = from->u8[1];
  packetbuf_copyfrom("ACK",3);
  unicast_send(&uc, &addr);
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data)
{
  PROCESS_EXITHANDLER(unicast_close(&uc);)

  PROCESS_BEGIN();

  printf("I'm the server, my rime addr is: %d.%d\n",
           rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[1]);
  unicast_open(&uc, 146, &unicast_callbacks);

  while(1)
  {
    static struct etimer et;

    etimer_set(&et, CLOCK_SECOND);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    printf("Total number of packets received = %d\n",numPkts);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

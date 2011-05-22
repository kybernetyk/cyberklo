/*
  this is the µIP app handler which is responsible to send data
  to our master server.
*/

#include "uip.h"
#include <string.h>
#include "udpapp.h"
#include "config.h"

extern int trigger;    //1 if triggered, 0 if not
extern unsigned long last_tick_count;

static struct udpapp_state s;

//configure our master server here
unsigned char master_ip[] = {10,0,1,2};
unsigned short master_port = 1234;

void dummy_app_appcall(void) {
}
struct uip_udp_conn *conn = NULL;

void udpapp_init(void) {
    s.state = 0;
  PT_INIT(&s.pt);
}

static PT_THREAD(handle_connection(void)) {
  PT_BEGIN(&s.pt);
  
  if (trigger == 1) {
    char data[32];
    sprintf(data,"%lu\n", last_tick_count);
    uip_send(data, strlen(data));
    trigger=0;
  }
  PT_END(&s.pt);
}

void udpapp_appcall(void) {
  if (uip_poll()) {
    handle_connection();
  }
  if (uip_closed() || uip_aborted() || uip_timedout()) {
   uip_udp_remove(conn);
   conn = NULL;
  }
}

//if we're not connected connect
void connect_to_master(void) {
  if (conn == NULL) {
    conn = uip_udp_new(master_ip, htons(master_port));
  }
}

/* udp flow:

  1. uip_udp_new()
  2. in udp_appcal:
    uip_poll() -> uip_send()
    uip_newdata() -> process data
*/

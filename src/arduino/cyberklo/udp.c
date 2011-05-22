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
  conn = uip_udp_new(master_ip, htons(master_port));
}

void  handle_connection(void) {
  //send this 2 times beaucse the first packets get lost oO
  if (s.state < 2 ) {
    char d[] = "switch sensor online\n";
    uip_send(d, strlen(d));
    s.state++;
    return;
  } 
  
  if (trigger == 1) {
    char data[32];
    sprintf(data,"%i, %lu\n", s.state, last_tick_count);
    uip_send(data, strlen(data));
    trigger=0;
  }
}

void udpapp_appcall(void) {
  if (uip_poll() || uip_rexmit() || uip_newdata()) {
    handle_connection();
  }
  if (uip_closed() || uip_aborted() || uip_timedout()) {
   uip_udp_remove(conn);
   conn = uip_udp_new(master_ip, htons(master_port));
  }
}


/* udp flow:

  1. uip_udp_new()
  2. in udp_appcal:
    uip_poll() -> uip_send()
    uip_newdata() -> process data
*/

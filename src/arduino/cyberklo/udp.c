/*
  this is the µIP app handler which is responsible to send data
  to our master server.
*/

#include "uip.h"
#include <string.h>
#include "udpapp.h"
#include "config.h"
#include "benc.h"

extern int trigger;    //1 if triggered, 0 if not
extern unsigned long last_tick_count;

static struct udpapp_state s;


void sprint(const char *format, ...);

//configure our master server here
unsigned char master_ip[] = {10,0,1,2};
unsigned short master_port = 1234;

void dummy_app_appcall(void) {
}
struct uip_udp_conn *conn = NULL;

void udpapp_init(void) {
  s.state = 0;
  conn = uip_udp_new(master_ip, htons(master_port));
  sprint("udp shit on!\n");
}

void  handle_connection(void) {
  //send this 2 times beaucse the first packets get lost oO
  if (s.state < 2 ) {
  char d[256];
  struct benc_t dict = benc_new_mem(d, 255);
  benc_dict_start(&dict);
  benc_dict_kv_i(&dict,"version",1);
  benc_dict_kv_s(&dict,"name", "klotrigger");
  benc_dict_kv_s(&dict,"type", "switch");
  benc_dict_kv_s(&dict,"payload", "register");
  benc_dict_end(&dict);
  sprint(dict.buf);
   
  uip_send(dict.buf, strlen(dict.buf));
  s.state++;
    return;
  } 
  
  if (trigger == 1) {
    char d[256];
    char e[32];
    
    struct benc_t payload = benc_new_mem(e,32);
    benc_dict_start(&payload);
    benc_dict_kv_i(&payload, "state", 1);
    benc_dict_end(&payload);

    struct benc_t dict = benc_new_mem(d, 256);
    benc_dict_start(&dict);
    benc_dict_kv_i(&dict,"version",1);
    benc_dict_kv_s(&dict,"name", "klotrigger");
    benc_dict_kv_s(&dict,"type", "switch");
    benc_dict_kv(&dict,"data", &payload);
    benc_dict_end(&dict);
    sprint(dict.buf);
    
    uip_send(dict.buf, strlen(dict.buf));
    trigger=0;
  }
}

void handle_data(void) {
    //PRINTF_SERIAL("data: %s\n", uip_appdata); 
    sprint(uip_appdata);
}

void udpapp_appcall(void) {
  if (uip_poll() || uip_rexmit()) {
    handle_connection();
  }
  if (uip_newdata()) {
    handle_data(); 
  }
  if (uip_closed() || uip_aborted() || uip_timedout()) {
   uip_udp_remove(conn);
   conn = uip_udp_new(master_ip, htons(master_port));
   if (conn) {
     uip_udp_bind(conn, HTONS(1234));
  }

  }
}


/* udp flow:

  1. uip_udp_new()
  2. in udp_appcal:
    uip_poll() -> uip_send()
    uip_newdata() -> process data
*/

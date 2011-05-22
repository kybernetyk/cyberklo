/*
	 cyberklo - the arduino part
	 we read here sensor data and forward them over UDP to our computer which will process them.
	 nothing fancy.

	 this "sketch" uses the WiShield library, which is based on µIP ... hence the strange code.
	 (never used uip before)

	 configure your wifi data below,
	 then in udp.c configure the address of your "master server" which processes the sensor data.

	 don't forget to:
	 uip-conf.h:
		#define UIP_CONF_UDP             1
		apps-conf.h:
		//#define APP_WEBSERVER
		//#define APP_WEBCLIENT
		//#define APP_SOCKAPP
->    #define APP_UDPAPP
		//#define APP_WISERVER
 */

#include <WiShield.h>

#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2

// Wireless configuration parameters ----------------------------------------
unsigned char local_ip[] = {10,0,1,23};	// IP address of WiShield
unsigned char gateway_ip[] = {10,0,1,1};	// router or gateway IP address
unsigned char subnet_mask[] = {255,255,255,0};	// subnet mask for the local network
const prog_char ssid[] PROGMEM = {"cylonsix"};		// max 32 bytes

unsigned char security_type = 3;	// 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {"fuerte1939"};	// max 64 characters

// WEP 128-bit keys
// sample HEX keys
prog_uchar wep_keys[] PROGMEM = {	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,	// Key 0
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00,	// Key 1
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00,	// Key 2
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00	// Key 3
};

// setup the wireless mode
// infrastructure - connect to AP
// adhoc - connect to another WiFi device
unsigned char wireless_mode = WIRELESS_MODE_INFRA;
unsigned char ssid_len;
unsigned char security_passphrase_len;

//---------------------------------------------------------------------------
//setting this to one will tell the udp-network-struff to send a message to our control center
int trigger = 0;

//timing shit
unsigned long last_tick_count = 0;
unsigned long now = 0;
unsigned long tick_count = 0;
#define TRIGGER_DELAY 5000      //time in ms to wait after trigger was fired before we send another trigger event

void setup()
{
	Serial.begin(9600);
	Serial.println("init start ...");
	WiFi.init();
	Serial.println("init done ...");
	trigger = 0;
	last_tick_count = millis();
	tick_count = 0;
}

void loop()
{
	//µip black magic
	WiFi.run();

	//update time keeping
	now = millis();
	tick_count += now - last_tick_count;
	last_tick_count = now;

	//after our switch was closed wait N ms before we fire another time        
	if (tick_count >= TRIGGER_DELAY) {
		trigger = 1;
		tick_count = 0;
                char d[128];
                sprintf(d,"%lu trigger!", last_tick_count);
		Serial.println(d);
	}
}

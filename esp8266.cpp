#include "util.h"
#include "esp8266.h"

WiFiUDP Udp;
char mac[7];

void hw_init() {
   wifi_fpm_do_wakeup();
   wifi_fpm_close();
   wifi_set_opmode(STATION_MODE);
   wifi_station_connect();

}

int init_wifi() {

	delay(1);

	WiFi.persistent(false);

//	if(wifi_fpm_get_sleep_type() != NONE_SLEEP_T) {
//		serial_printf("sleep mode: %u\r\n", wifi_fpm_get_sleep_type());
//		serial_printf("back from sleep...\r\n");
//		ESP.deepSleep(10e6, RF_CAL);
//	}

	int n = WiFi.scanNetworks();

	for (int i = 0; i < n; i++) {
	  serial_printf("found SSID: %s\r\n", WiFi.SSID((uint8_t)i).c_str());
	}

	unsigned long startTime = millis();

	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

	DEBUGP("Connecting to wifi...");
	while (WiFi.status() != WL_CONNECTED && millis() - startTime < WIFI_TIMEOUT) {
		delay(500);
		DEBUGP(".");
	}

	if(WiFi.status() == WL_CONNECTED) {

		DEBUGP("Connected, IP address: %s\r\n", WiFi.localIP().toString().c_str());
		return 1;

	} else {

		DEBUGP("Wifi connection timed out...\r\n");
		return -1;
	}

}

void wifi_pause() {

	WiFi.disconnect( true );
	WiFi.mode(WIFI_OFF);
	delay( 1 );

}

int wifi_send_data(uint8_t *data, int len) {

	int result = 0;
	IPAddress broadcast_addr;
	if (!broadcast_addr.fromString(BROADCAST_ADDR)) {
		DEBUGP("Couldn't parse IP address %s\r\n", BROADCAST_ADDR);
	}

	Udp.beginPacket(broadcast_addr, BROADCAST_PORT);
	Udp.write(data, len);
	result = Udp.endPacket();
	return result;

}

const char *wifi_get_mac() {

	strcpy(mac, WiFi.macAddress().c_str());
	return mac;

}

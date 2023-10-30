#include <Arduino.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "settings.h"
#include "util.h"

#include "esp8266.h"

ADC_MODE(ADC_VCC);

#define LIMON_VOLTAGE_FRAME 0x4ff

HardwareSerial console = Serial;

#define SLEEP_MAX 7200e6
#define SLEEP_CYCLES_MAX 100
#define VOLTAGE_OFFSET 700

struct {
	uint32_t crc32;
	uint32_t sleepTimes;
} rtcState;

int wifiAvailable = 0;

void checkIfSleep();
void deepSleep(uint64_t amount, RFMode mode);

void setup() {

	console.begin(9600);
	console.setDebugOutput(DEBUG_ESP);

	serial_printf("LiMon initializing...\r\n");

	checkIfSleep();

	serial_printf("LiMon MAC: %s\r\n", wifi_get_mac());

	if ((wifiAvailable = (init_wifi() > 0))) {
		serial_printf("Wifi initialized\r\n");
	} else {
		serial_printf("Wifi initialization error\r\n");
	}

	serial_printf("LiMon initialized...\r\n");

}

void loadState() {

	ESP.rtcUserMemoryRead(0, (uint32_t*) &rtcState, sizeof(rtcState));

	uint32_t crc = calculateCRC32(((uint8_t*) &rtcState) + 4,
			sizeof(rtcState) - 4);

	if (rtcState.crc32 != crc) {
		serial_printf("invalid crc, resetting state\r\n");
		bzero(&rtcState, sizeof(rtcState));
	}

}

void saveState() {

	rtcState.crc32 = calculateCRC32(((uint8_t*) &rtcState) + 4,
			sizeof(rtcState) - 4);
	ESP.rtcUserMemoryWrite(0, (uint32_t*) &rtcState, sizeof(rtcState));

}

void checkIfSleep() {

	loadState();

	if (rtcState.sleepTimes > 0 && rtcState.sleepTimes < SLEEP_CYCLES_MAX) {

		rtcState.sleepTimes--;

		serial_printf("%u times left to sleep...\r\n", rtcState.sleepTimes);

		saveState();

		serial_printf("sleeping...");
		deepSleep(SLEEP_MAX, rtcState.sleepTimes > 0 ? RF_DISABLED : RF_DEFAULT);

	} else if (rtcState.sleepTimes > SLEEP_CYCLES_MAX) {
		serial_printf("odd sleep times %u, ignoring...\r\n",
				rtcState.sleepTimes);
	}

}

void deepSleep(uint64_t amount, RFMode mode) {

	wifi_pause();
	DEBUGP("deepSleep(%llu, %u)\r\n", amount, mode);
	ESP.deepSleep(amount, mode);
	delay(1000);

}

void loop() {

	struct {
		uint32_t id;
		uint8_t can_frame[4];
	} datagram;

	uint16_t adjustedVoltage = 0;

	datagram.id = LIMON_VOLTAGE_FRAME;

	serial_printf("Supply voltage: %.2f\r\n", (float)ESP.getVcc()/1000);

	adjustedVoltage = ESP.getVcc() + VOLTAGE_OFFSET;

	serial_printf("Adjusted voltage: %.2f\r\n", (float)adjustedVoltage/1000);

	((uint32_t*) datagram.can_frame)[0] = adjustedVoltage;

	if(wifiAvailable) {

		DEBUGP("sending report...\r\n");

		for (int i = 0; i < WIFI_REPORT_COUNT; i++) {

			wifi_send_data((uint8_t*) &datagram, sizeof(datagram));
			delay(random(50, 500));

		}

	} else {

		DEBUGP("Wifi is not available, skipping report\r\n");

	}

	uint64_t sleep_time = SLEEP_TIME;

	if (sleep_time * 1e6 > SLEEP_MAX) {

		rtcState.sleepTimes = sleep_time * 1e6 / SLEEP_MAX;
		saveState();
		deepSleep(SLEEP_MAX, RF_DISABLED);

	} else {
		deepSleep(sleep_time * 1e6, RF_DEFAULT);
	}

}

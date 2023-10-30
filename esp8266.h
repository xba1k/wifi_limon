#ifndef ESP8266_H
#define ESP8266_H

#include <Arduino.h>
#include "settings.h"

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <IPAddress.h>
extern WiFiUDP Udp;

int init_wifi();
void wifi_pause();
int wifi_send_data(uint8_t *data, int len);
const char *wifi_get_mac();

#endif

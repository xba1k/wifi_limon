# LiMon

## Overview

If you're like me, you may have a bunch of Lithium Ion cells either laying around waiting for the future projects, or already installed in a variety of equipment. It's easy to forget where they all are, and what state they're in. The purpose of this project is to create a simple and energy efficient device that is powered by the battery cells and reports its voltage over wifi without much drain over a long period of time. To accomplish that an ESP8266 microcontroller is programmed to remain in deep sleep and only report the voltage once a day.

## Setup

This code was created using Eclipse IDE with Arduino plugin. Eclipse no longer maintains that plugin, and it's no longer available for download from Eclipse Marketplace. The code can be compiled using Arduino IDE by concatenating all project files. 

See settings.h for wifi credentials and various other parameters.

# Hardware

The code will run on all ESP8266 boards, but to utilize the deep sleep WAKE pin has to be exposed and connected to RST. Thus the boards such as ESP-01 shouldn't be used. Pictured below is the ESP-12E.

Pinout:

CH_EN -> 10K -> VCC
RST -> GPIO16 (WAKE)
GPIO0 -> 10K -> VCC
GPIO15 -> 10K -> GND

![alt text](https://github.com/xba1k/wifi_limon/blob/main/LiMon.jpg?raw=true)

## Receiver

Example Python script is provided. The script has to run with root privileges in order to access MAC addresses of the source packets. Scapy Python library is used for parsing the packets.

## Demo

```
sudo tcpdump -e -n -A -x ip and udp and port 1080
tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
listening on eno1, link-type EN10MB (Ethernet), capture size 262144 bytes```

```
sudo ./limon_receiver.py 
waiting for reports...
```

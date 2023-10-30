#!/usr/bin/python3

import socket
import logging
import sys
from scapy.layers.l2 import Ether
from scapy.layers.inet import IP,UDP
from datetime import datetime
from struct import unpack

IFACE = sys.argv[1] if len(sys.argv) == 2 else "eno1"
UDP_PORT = 1080
IP_DST = "255.255.255.255"

VOLTAGE_FRAME = 0x4ff
ETH_P_ALL = 3

def parse_voltage(frameId, frameData):
    batteryVoltage = unpack("<H", frameData[:2])
    return "{:.2f}".format(batteryVoltage[0] / 1000.0)

frame_types = {
    VOLTAGE_FRAME: lambda frameId, frameData: parse_voltage(frameId, frameData),
    0: lambda frameId, frameData: "unknown frame type {}".format(hex(frameId))
}

def parse_frame(data):
    frameId = unpack('<I', data[0:4])[0]

    if frameId in frame_types:
        return frame_types[frameId](frameId, data[4:])
    else:
        return frame_types[0](frameId, data[4:])

def load_inventory():

    result = {}
    infile = open("inventory.txt", "r")

    for line in infile:
        result[line[:17]] = line[17:].strip()

    return result


logging.basicConfig(filename="limon_receiver.log",
                    filemode='a',
                    format='%(asctime)s,%(msecs)d %(message)s',
                    datefmt='%Y-%m-%d %H:%M:%S',
                    level=logging.INFO)

inventory = load_inventory()

s = socket.socket(socket.AF_PACKET, socket.SOCK_RAW, socket.htons(ETH_P_ALL))
s.bind((IFACE, 0))

print("waiting for reports...")

prevPayload = None
prevTime = None

while True:
    data = s.recv(2048)
    frame = Ether(data)

    if frame.haslayer(IP) and frame.haslayer(UDP) and frame[IP].dst == IP_DST and frame[UDP].dport == UDP_PORT and (frame[UDP].payload != prevPayload or time.time() - prevTime > 60):

        #frame.show()

        label = inventory[frame.src] if inventory.get(frame.src) is not None else "Unlisted"
        voltage = parse_frame(bytes(frame[UDP].payload))

        print("{} {}/{} [{}] volts: {}".format(datetime.now(), frame.src, frame[IP].src, label, voltage))

        logging.info("{} [{}] {}".format(frame.src, label, voltage))

        prevPayload = frame[UDP].payload
        prevTime = time.time()

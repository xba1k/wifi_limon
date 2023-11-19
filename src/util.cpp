#include "util.h"

#include <Arduino.h>

#if DEBUG
char _debug_buf[DEBUG_BUF_SIZE];
#endif

void serial_bprintf(char *buf, const char *fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  vsprintf(buf, fmt, argp);
  STDOUT.print(buf);
  va_end(argp);
}

void serial_sbprintf(HardwareSerial *serial, char *buf, const char *fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  vsprintf(buf, fmt, argp);
  serial->print(buf);
  va_end(argp);
}

void serial_printf(const char *fmt, ...) {
  char buf[MAX_OUT_BUF_SIZE];
  va_list argp;
  va_start(argp, fmt);
  vsprintf(buf, fmt, argp);
  STDOUT.print(buf);
  va_end(argp);
}

char *ltrim(char *buf) {
  char *p, *p2;

  for (p = buf; (*p == '\r' || *p == '\n') && *p != '\0'; p++)
    ;

  for (p2 = p; *p2 != '\0'; p2++) {
    buf[p2 - p] = *p2;
  }

  buf[p2 - p] = '\0';
  return buf;
}

char *rtrim(char *buf) {
  char *p = buf + strlen(buf);

  for (; p >= buf && (*p == '\r' || *p == '\n' || *p == '\0'); p--)
    ;

  if ((*(p + 1) == '\r' || *(p + 1) == '\n')) *(p + 1) = '\0';
  return buf;
}

uint32_t calculateCRC32(const uint8_t *data, size_t length) {
  uint32_t crc = 0xffffffff;
  while (length--) {
    uint8_t c = *data++;
    for (uint32_t i = 0x80; i > 0; i >>= 1) {
      bool bit = crc & 0x80000000;
      if (c & i) {
        bit = !bit;
      }
      crc <<= 1;
      if (bit) {
        crc ^= 0x04c11db7;
      }
    }
  }
  return crc;
}

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char *sbrk(int incr);
#else   // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
#ifndef ESP8266
  char top;
#endif
#ifdef ESP8266
  return ESP.getFreeHeap();
#elif __arm__
  return &top - reinterpret_cast<char *>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else   // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

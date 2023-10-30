#ifndef UTIL_H
#define UTIL_H

#include <stdarg.h>
#include <string.h>
#include <Arduino.h>
#include "settings.h"

#define DEBUG 1
#define DEBUG_ESP false

#ifndef STDOUT
extern HardwareSerial console;
#define STDOUT console
#endif

#ifndef MAX_OUT_BUF_SIZE
#define MAX_OUT_BUF_SIZE 128
#endif

#ifdef DEBUG

#ifndef DEBUG_BUF_SIZE
#define DEBUG_BUF_SIZE 128
#endif

#ifndef DEBUG_BUF
#define DEBUG_BUF
extern char _debug_buf[DEBUG_BUF_SIZE];
#endif

#define DEBUGP(...) do { serial_bprintf(_debug_buf, __VA_ARGS__); } while(false)
#else
#define DEBUGP(...) do{} while(false)
#endif

#define uint32Value(x, y) (x << 16 | y)
#define floatValue(x) (*(float*)x)
#define int8LsbValue(x) ((uint8_t)x)
#define int8MsbValue(x) ((uint8_t) x >> 8)
#define uint8LsbValue(x) int8LsbValue(x)
#define uint8MsbValue(x) int8MsbValue(x)

void serial_printf(const char *fmt, ...);
void serial_bprintf(char *buf, const char *fmt, ...);
void serial_sbprintf(HardwareSerial *serial, char *buf, const char *fmt, ...);
char* ltrim(char *buf);
char* rtrim(char *buf);
int freeMemory();

uint32_t calculateCRC32(const uint8_t *data, size_t length);

#endif

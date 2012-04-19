#ifdef __cplusplus
extern "C"
{
#endif

#ifndef HEXDUMP_H
#define HEXDUMP_H
// Simple hexdumper, taken from:
// http://stackoverflow.com/questions/29242/off-the-shelf-c-hex-dump-code

#include <stdio.h>
#include <stdint.h>

void hexdump(const void *ptr, size_t len);

#endif

#ifdef __cplusplus
}
#endif

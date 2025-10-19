#ifndef CRC64_H
#define CRC64_H

#include <stddef.h>
#include <stdint.h>

uint64_t crc64_be(uint64_t crc, const void *p, size_t len);

uint64_t crc64(const void *data, size_t len);

#endif // CRC64_H

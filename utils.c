#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "utils.h"

void print_results(const char *title, double elapsed, uint64_t hash) {
    printf("%s\n", title);
    printf("Time elapsed: %lf s\n", elapsed);
    printf("CRC64 calculated: 0x%" PRIX64 "\n", hash);
}

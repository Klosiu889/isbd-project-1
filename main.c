#include <stdint.h>
#include <stdio.h>

#include "mmap_strategy.h"
#include "read_strategy.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];

    read_sequential(filename);
    read_random(filename);

    mmap_sequential(filename);
    mmap_random(filename);

    return 0;
}

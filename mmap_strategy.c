#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "const.h"
#include "crc64.h"
#include "mmap_strategy.h"
#include "utils.h"

void mmap_sequential(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("file failed to open properly");
        exit(EXIT_FAILURE);
    }

    struct stat st;
    fstat(fd, &st);
    off_t size = st.st_size;

    struct timespec start, end;

    uint64_t crc = 0;
    off_t offset = 0;

    unsigned char *data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        close(fd);
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock gettime start");
        exit(EXIT_FAILURE);
    }

    while (offset < size) {
        off_t chunk =
            (offset + BLOCK_SIZE <= size) ? BLOCK_SIZE : (size - offset);
        crc = crc64_be(crc, data + offset, chunk);
        offset += BLOCK_SIZE;
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock gettime end");
        exit(EXIT_FAILURE);
    }

    double elapsed =
        (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;

    print_results("Mmap sequential", elapsed, crc);

    munmap(data, size);
    close(fd);
}

void mmap_random(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("file failed to open properly");
        exit(EXIT_FAILURE);
    }

    struct stat st;
    fstat(fd, &st);
    off_t size = st.st_size;

    struct timespec start, end;

    uint64_t crc = 0;
    off_t front = 0;
    off_t back = (size > BLOCK_SIZE) ? size - BLOCK_SIZE : 0;

    unsigned char *data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        close(fd);
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock gettime start");
        exit(EXIT_FAILURE);
    }

    while (front <= back) {
        off_t front_chunk =
            (front + BLOCK_SIZE <= size) ? BLOCK_SIZE : size - front;
        crc = crc64_be(crc, data + front, front_chunk);

        if (back > front) {
            off_t back_chunk =
                (back + BLOCK_SIZE <= size) ? BLOCK_SIZE : size - back;
            crc = crc64_be(crc, data + back, back_chunk);
        }

        front += BLOCK_SIZE;
        if (back >= BLOCK_SIZE)
            back -= BLOCK_SIZE;
        else
            break;
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock gettime end");
        exit(EXIT_FAILURE);
    }

    double elapsed =
        (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;

    print_results("Mmap random", elapsed, crc);

    munmap(data, size);
    close(fd);
}

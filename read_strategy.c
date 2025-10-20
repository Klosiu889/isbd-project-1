#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "const.h"
#include "crc64.h"
#include "read_strategy.h"
#include "utils.h"

void read_sequential(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("file failed to open properly");
        exit(EXIT_FAILURE);
    }

    struct timespec start, end;

    uint64_t crc = 0;
    unsigned char *buffer =
        (unsigned char *)malloc(BLOCK_SIZE * sizeof(unsigned char));
    if (buffer == NULL) {
        exit(EXIT_FAILURE);
    }

    ssize_t bytes;

    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock gettime start");
        exit(EXIT_FAILURE);
    }

    while ((bytes = read(fd, buffer, BLOCK_SIZE)) > 0) {
        crc = crc64_be(crc, buffer, bytes);
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock gettime end");
        exit(EXIT_FAILURE);
    }

    double elapsed =
        (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;

    print_results("Read sequential", elapsed, crc);

    close(fd);
    free(buffer);
}

void read_random(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("file failed to open properly");
        exit(EXIT_FAILURE);
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        close(fd);
        perror("fstat");
        exit(EXIT_FAILURE);
    }
    off_t size = st.st_size;

    struct timespec start, end;

    uint64_t crc = 0;
    unsigned char *buffer =
        (unsigned char *)malloc(BLOCK_SIZE * sizeof(unsigned char));
    if (buffer == NULL) {
        exit(EXIT_FAILURE);
    }

    off_t front = 0;
    off_t back = size - BLOCK_SIZE;

    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock gettime start");
        exit(EXIT_FAILURE);
    }

    while (front <= back) {
        lseek(fd, front, SEEK_SET);
        ssize_t front_bytes = read(fd, buffer, BLOCK_SIZE);
        if (front_bytes > 0)
            crc = crc64_be(crc, buffer, front_bytes);

        if (back > front) {
            lseek(fd, back, SEEK_SET);
            ssize_t back_bytes = read(fd, buffer, BLOCK_SIZE);
            if (back_bytes > 0)
                crc = crc64_be(crc, buffer, back_bytes);
        }

        front += BLOCK_SIZE;
        back -= BLOCK_SIZE;
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock gettime end");
        exit(EXIT_FAILURE);
    }

    double elapsed =
        (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;

    print_results("Read random", elapsed, crc);

    close(fd);
    free(buffer);
}

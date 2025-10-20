CC = gcc

CFLAGS = -Wall -Werror -g

TARGET = read_mmap_test
SRCS = main.c crc64.c read_strategy.c mmap_strategy.c utils.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

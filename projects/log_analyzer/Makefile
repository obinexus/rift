CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I/usr/local/include -I$(LIBRIFT_DIR)/include -I$(LIBRIFT_DIR)/include/librift
LDFLAGS = -L/usr/local/lib -L$(LIBRIFT_DIR) -lrift

# Define LibRift directory (adjust as needed)
LIBRIFT_DIR = ../../build

TARGET = log_analyzer
SRCS = log_analyzer.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET) sample_log.txt
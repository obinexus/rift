CC = gcc
CFLAGS = -Wall -Wextra -g -I../../include
LDFLAGS = -L../../build
LDLIBS = -lrift -lm

SRC = string_pattern_detector.c
OBJ = $(SRC:.c=.o)
TARGET = pattern_detector

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean

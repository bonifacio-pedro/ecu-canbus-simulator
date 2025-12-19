CC = gcc

CFLAGS = -Wall -Wextra -O2 -std=c99

SRCS = ./src/main.c ./src/ecu.c ./src/can.c

OBJS = $(SRCS:.c=.o)

TARGET = ecu_sim

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)
CC=gcc
CFLAGS=-O2 -Wall -std=c99 -I/usr/local/include
LDFLAGS=-L/usr/local/lib -lrgbmatrix -lm -lpthread -lbme280

# Target executable name
TARGET=temperature_display

# Source files
SRC=temperature_display.c

# Object files
OBJ=$(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean

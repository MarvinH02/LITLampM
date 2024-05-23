CXXFLAGS=-Wall -O3 -g -Wextra -Wno-unused-parameter -I/home/g10/rpi-rgb-led-matrix/include
LDFLAGS=-L/home/g10/rpi-rgb-led-matrix/lib -lrgbmatrix -lrt -lm -lpthread

all: snake

snake: snake.o
	$(CXX) $< -o $@ $(LDFLAGS)

snake.o: snake.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f snake snake.o

.PHONY: all clean

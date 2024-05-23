LDFLAGS+=-L/usr/local/lib -lrgbmatrix
CXXFLAGS+=-I/usr/local/include -std=c++11 -O3

all: date_display

date_display: date_display.o
	$(CXX) date_display.o -o date_display $(LDFLAGS)

date_display.o: date_display.cc
	$(CXX) -c date_display.cc -o date_display.o $(CXXFLAGS)

clean:
	rm -f date_display date_display.o

.PHONY: all clean

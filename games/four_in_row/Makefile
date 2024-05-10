CXXFLAGS=-Wall -O3 -g -Wextra -Wno-unused-parameter -std=c++11
LDFLAGS=-lrt -lm -lpthread
RGB_INCDIR=../rpi-rgb-led-matrix/include
RGB_LIBDIR=../rpi-rgb-led-matrix/lib
RGB_LIBRARY_NAME=rgbmatrix
RGB_LIBRARY=$(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a

four_in_row: four_in_row.o
	$(CXX) four_in_row.o -o $@ $(RGB_LIBRARY) $(LDFLAGS)

four_in_row.o: four_in_row.cc
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f four_in_row four_in_row.o

.PHONY: clean

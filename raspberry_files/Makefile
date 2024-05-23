RGB_INCDIR=/home/g10/rpi-rgb-led-matrix/include
RGB_LIBDIR=/home/g10/rpi-rgb-led-matrix/lib
RGB_LIBRARY_NAME=rgbmatrix
RGB_LIBRARY=$(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a

# Compiler settings for C++ code
CXX ?= g++
CXXFLAGS += -Wall -O3 -g -Wextra -Wno-unused-parameter -I$(RGB_INCDIR)
LDFLAGS += -L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -lrt -lm -lpthread

all : DisplayColor

DisplayColor: DisplayColor.o $(RGB_LIBRARY)
	$(CXX) DisplayColor.o -o DisplayColor $(LDFLAGS)

DisplayColor.o: DisplayColor.cc
	$(CXX) $(CXXFLAGS) -c DisplayColor.cc -o DisplayColor.o

$(RGB_LIBRARY):
	$(MAKE) -C $(RGB_LIBDIR)

clean:
	rm -f DisplayColor DisplayColor.o
	$(MAKE) -C $(RGB_LIBDIR) clean

.PHONY: all clean

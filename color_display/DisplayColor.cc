#include "led-matrix.h"
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>

using namespace rgb_matrix;

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}

int main(int argc, char *argv[]) {
    RGBMatrix::Options matrix_options;
    RuntimeOptions runtime_opt;

    matrix_options.rows = 64;
    matrix_options.cols = 64;
    matrix_options.chain_length = 1;
    matrix_options.parallel = 1;
    matrix_options.hardware_mapping = "regular"; 

    RGBMatrix *matrix = RGBMatrix::CreateFromOptions(matrix_options, runtime_opt);
    if (matrix == NULL) {
        fprintf(stderr, "Failed to create matrix object.\n");
        return 1;
    }

    unsigned int red;
    unsigned int green;
    unsigned int blue;
    std::string colorHex;
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--color", 7) == 0 && i + 1 < argc) {
            colorHex = argv[i + 1];
            unsigned int color = std::stoul(colorHex, nullptr, 16);
            red = (color >> 16) & 0xFF;
            green = (color >> 8) & 0xFF;
            blue = color & 0xFF;
            break;
        }
    }
    signal(SIGTERM, InterruptHandler);
    signal(SIGINT, InterruptHandler);
  
    matrix->Fill(red, green, blue);

    while (!interrupt_received) {
        usleep(100 * 1000); 
    }

    delete matrix;
    return 0;
}

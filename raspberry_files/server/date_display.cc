#include "led-matrix.h"
#include "graphics.h"
#include <time.h>
#include <stdio.h>
#include <unistd.h>

using namespace rgb_matrix;

int main(int argc, char *argv[]) {
    RGBMatrix::Options matrix_options;
    rgb_matrix::RuntimeOptions runtime_opt;

    matrix_options.rows = 64; 
    matrix_options.cols = 64;
    matrix_options.chain_length = 1;
    matrix_options.parallel = 1;
    matrix_options.hardware_mapping = "regular";

    RGBMatrix *matrix = RGBMatrix::CreateFromOptions(matrix_options, runtime_opt);
    if (matrix == NULL) {
        fprintf(stderr, "Failed to create matrix\n");
        return 1;
    }

    Font font;
    if (!font.LoadFont("../fonts/8x13.bdf")) {
        fprintf(stderr, "Failed to load font\n");
        return 1;
    }

    Color color(255, 255, 0);
    FrameCanvas *offscreen = matrix->CreateFrameCanvas();

    while (true) {
        time_t rawtime;
        struct tm *timeinfo;
        char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

        offscreen->Clear();
        DrawText(offscreen, font, 0, 10, color, NULL, buffer);
        offscreen = matrix->SwapOnVSync(offscreen);

        sleep(1);
    }

    delete matrix;
    return 0;
}

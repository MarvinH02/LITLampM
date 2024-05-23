#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <bcm2835.h>
#include <led-matrix.h>
#include <graphics.h>

using namespace rgb_matrix;
using namespace std;

int main(int argc, char *argv[]) {
    RGBMatrix::Options matrix_options;
    RuntimeOptions runtime_opt;

    matrix_options.rows = 64;
    matrix_options.cols = 64;
    matrix_options.chain_length = 1;
    matrix_options.parallel = 1;
    matrix_options.hardware_mapping = "regular";

    RGBMatrix *matrix = RGBMatrix::CreateFromOptions(matrix_options, runtime_opt);
    if (matrix == nullptr) {
        cerr << "Failed to create matrix. Check hardware configuration." << endl;
        return 1;
    }

    FrameCanvas *canvas = matrix->CreateFrameCanvas();
    Font font;
    if (!font.LoadFont("/home/g10/rpi-rgb-led-matrix/fonts/4x6.bdf")) {
        cerr << "Could not load font" << endl;
        return 1;
    }

    Color color(255, 255, 255);
    int y_position = 10;  

    while (true) {
        ifstream file("weather_data.txt");
        if (!file) {
            cerr << "Could not open file 'weather_data.txt'" << endl;
            return 1;
        }

        string line;
        canvas->Clear();

        while (getline(file, line)) {
            if (line.empty()) continue;
            DrawText(canvas, font, 0, y_position, color, line.c_str());
            y_position += 10;
        }

        canvas = matrix->SwapOnVSync(canvas);
        usleep(10000000);  
        y_position = 10;
    }

    delete matrix;
    return 0;
}
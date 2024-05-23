#include <stdio.h>
#include <iostream>
#include <bcm2835.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <vector>
#include </home/g10/rpi-rgb-led-matrix/include/led-matrix.h>
#include </home/g10/rpi-rgb-led-matrix/include/graphics.h>

using namespace rgb_matrix;
using namespace std;

vector<string> split(const string &str, const string &delimiter) {
    vector<string> tokens;
    size_t start = 0, end = 0;
    while ((end = str.find(delimiter, start)) != string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
    }
    tokens.push_back(str.substr(start));
    return tokens;
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
    if (matrix == nullptr) {
        cerr << "Failed to create matrix. Check hardware configuration." << endl;
        return 1;
    }

    FrameCanvas *canvas = matrix->CreateFrameCanvas();
    Font font;
    if (!font.LoadFont("/home/g10/rpi-rgb-led-matrix/fonts/5x8.bdf")) {
        cerr << "Could not load font" << endl;
        return 1;
    }

    Color color(255, 255, 255);
    int y_position = 10;

    while (true) {
        ifstream file("sensor_data.txt");
        if (!file) {
            cerr << "Could not open file 'sensor_data.txt'" << endl;
            return 1;
        }

        string line;
        canvas->Clear();

        if (getline(file, line)) {
            if (!line.empty()) {
                vector<string> tokens = split(line, ", ");
                for (const string &token : tokens) {
                    DrawText(canvas, font, 0, y_position, color, token.c_str());
                    y_position += 10;
                }
            }
        }

        canvas = matrix->SwapOnVSync(canvas);
        usleep(1000000);
        y_position = 10;
    }

    delete matrix;
    return 0;
}

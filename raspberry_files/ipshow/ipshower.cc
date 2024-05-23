#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include </home/g10/rpi-rgb-led-matrix/include/led-matrix.h>
#include </home/g10/rpi-rgb-led-matrix/include/graphics.h>

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

    ifstream file("/home/g10/Desktop/server/ipshow/laniptest.txt");
    string line;
    if (!file) {
        cerr << "Could not open file 'laniptest.txt'" << endl;
        return 1;
    }

    Color color(255, 255, 255);
    int y_position = 10;
    int line_count = 0;
    const int display_duration = 20; // total display duration in seconds
    time_t start_time = time(nullptr);

    while (getline(file, line)) {
        size_t start = 0, end;
        string delimiter = ", ";
        canvas->Clear();

        while ((end = line.find(delimiter, start)) != string::npos) {
            string token = line.substr(start, end - start);
            DrawText(canvas, font, 0, y_position, color, token.c_str());
            y_position += 10;
            start = end + delimiter.length();
        }

        string token = line.substr(start);
        DrawText(canvas, font, 0, y_position, color, token.c_str());
        y_position += 10;

        canvas = matrix->SwapOnVSync(canvas);
        usleep(20000000); // 2 seconds delay
        y_position = 10;

        line_count++;
        if (line_count >= 10) {
            file.clear();  // Clear the EOF flag
            file.seekg(0, ios::beg);  // Go back to the beginning of the file
            line_count = 0;
        }

        if (difftime(time(nullptr), start_time) >= display_duration) {
            break;
        }
    }

    delete matrix;
    return 0;
}

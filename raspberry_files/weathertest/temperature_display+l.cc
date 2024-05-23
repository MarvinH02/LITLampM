#include <stdio.h>
#include <iostream>
#include <bcm2835.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include </home/g10/rpi-rgb-led-matrix/include/led-matrix.h>
#include </home/g10/rpi-rgb-led-matrix/include/graphics.h>

using namespace rgb_matrix;
using namespace std;

int main(int argc, char *argv[]) {
    RGBMatrix::Options matrix_options;
    RuntimeOptions runtime_opt;

    // Configure the matrix
    matrix_options.rows = 64;
    matrix_options.cols = 64;
    matrix_options.chain_length = 1;
    matrix_options.parallel = 1;
    matrix_options.hardware_mapping = "regular";

    // Create matrix
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

    ifstream file("sensor_data.txt");
    string line;
    if (!file) {
        cerr << "Could not open file 'sensor_data.txt'" << endl;
        return 1;
    }

    // Set color and vertical position for the text
Color color(255, 255, 255);  // white color
int y_position = 10;  
int line_count = 0;

while (getline(file, line)) {
	size_t start = 0, end;
	string delimiter = ", ";
	canvas->Clear();

	while ((end = line.find(delimiter, start)) != string::npos) {
            string token = line.substr(start, end - start);
            DrawText(canvas, font, 0, y_position, color, token.c_str());
            y_position += 10; // Move to the next line
            start = end + delimiter.length();
        }
		string token = line.substr(start);
		DrawText(canvas, font, 0, y_position, color, token.c_str());
		y_position += 10;

	// Show each line for 2 seconds
	canvas = matrix->SwapOnVSync(canvas);
	usleep(2000000);
	y_position = 10;

    line_count ++;
    if(line_count >= 10){
        file.seekg(0, ios::beg);
        line_count = 0;
    }
}

// Clean up
delete matrix;
    return 0;
}

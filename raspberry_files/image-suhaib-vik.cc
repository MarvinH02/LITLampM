#include <Magick++.h>
#include "led-matrix.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

using namespace std;
using namespace rgb_matrix;


bool FileExists(const std::string &filepath) {
    std::ifstream file(filepath);
    return file.good();
}

std::vector<unsigned char> base64_decode(const std::string &in) {
    static const std::string chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
    std::vector<unsigned char> bytes;
    int val = 0, valb = -8;
    for (unsigned char c : in) {
        if (isspace(c)) continue;
        auto pos = chars.find(c);
        if (pos == std::string::npos) break;
        val = (val << 6) + pos;
        valb += 6;
        if (valb >= 0) {
            bytes.push_back((val >> valb) & 0xFF);
            valb -= 8;
        }
    }
    return bytes;
}


void DisplayImageFromBase64(const char *base64_data, RGBMatrix *matrix) {
    std::vector<unsigned char> image_data = base64_decode(base64_data);

    
    std::string temp_image_path = "/tmp/temp_image.jpg";
    std::ofstream image_file(temp_image_path, std::ios::out | std::ios::binary);
    image_file.write(reinterpret_cast<const char*>(image_data.data()), image_data.size());
    image_file.close();

    if (!FileExists(temp_image_path)) {
        std::cerr << "Failed to write decoded image file.\n";
        return;
    }

    
    Magick::Image image;
    image.read(temp_image_path);
    image.scale(Magick::Geometry(matrix->width(), matrix->height()));

    auto canvas = matrix->CreateFrameCanvas();
    const int offset_x = 0, offset_y = 0;
    for (size_t y = 0; y < image.rows(); ++y) {
        for (size_t x = 0; x < image.columns(); ++x) {
            const Magick::Color &c = image.pixelColor(x, y);
            if (c.alphaQuantum() < 256) {
                canvas->SetPixel(x + offset_x, y + offset_y,
                                 c.redQuantum() * 255 / QuantumRange,
                                 c.greenQuantum() * 255 / QuantumRange,
                                 c.blueQuantum() * 255 / QuantumRange);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file with base64 image data>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Unable to open file: " << argv[1] << std::endl;
        return 1;
    }

    std::string base64_data((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());

    file.close();

    RGBMatrix::Options matrix_options;
    rgb_matrix::RuntimeOptions runtime_opt;
    RGBMatrix *matrix = RGBMatrix::CreateFromOptions(matrix_options, runtime_opt);
    if (matrix == NULL) {
        std::cerr << "Could not create matrix\n";
        return 1;
    }

    DisplayImageFromBase64(base64_data.c_str(), matrix);

    while (!interrupt_received) sleep(1);

    matrix->Clear();
    delete matrix;

    return 0;
}
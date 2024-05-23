#include "led-matrix.h"
#include "graphics.h"
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace rgb_matrix;

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}

class DemoRunner {
protected:
  DemoRunner(Canvas *canvas) : canvas_(canvas) {}
  inline Canvas *canvas() { return canvas_; }

public:
  virtual ~DemoRunner() {}
  virtual void Run() = 0;

private:
  Canvas *const canvas_;
};


class ColorDisplay : public DemoRunner {
public:
  ColorDisplay(Canvas *canvas, uint32_t color = 0xFF0000)  
      : DemoRunner(canvas), color_(color) {}

  void Run() override {
    uint8_t r = (color_ >> 16) & 0xFF;
    uint8_t g = (color_ >> 8) & 0xFF;
    uint8_t b = color_ & 0xFF;

    while (!interrupt_received) {
      canvas()->Fill(r, g, b);
      usleep(1000 * 1000); 
    }
  }

private:
  uint32_t color_;
};

int main(int argc, char *argv[]) {
  RGBMatrix::Options matrix_options;
  RuntimeOptions runtime_options;


  RGBMatrix *matrix = RGBMatrix::CreateFromOptions(matrix_options, runtime_options);
  if (matrix == NULL) {
    fprintf(stderr, "Creating matrix failed\n");
    return 1;
  }

  Canvas *canvas = matrix;


  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  DemoRunner *demo_runner = new ColorDisplay(canvas); 
  demo_runner->Run();

  delete demo_runner;
  delete canvas;
  return 0;
}
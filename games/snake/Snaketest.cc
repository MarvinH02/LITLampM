#include "led-matrix.h"
#include "graphics.h"
#include <unistd.h>
#include <termios.h>
#include <deque>
#include <signal.h>
#include <fcntl.h>
#include <vector>
#include <iostream>
#include <math.h>

using namespace std;
using namespace rgb_matrix;

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}

static int kbhit(void) {
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSADRAIN, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF) {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  RGBMatrix::Options matrix_options;
  rgb_matrix::RuntimeOptions runtime_opt;

  matrix_options.rows = 64;
  matrix_options.cols = 64;
  matrix_options.chain_length = 1;
  matrix_options.parallel = 1;
  matrix_options.hardware_mapping = "regular";  // or adafruit-hat, regular-pi1

  RGBMatrix *matrix = RGBMatrix::CreateFromOptions(matrix_options, runtime_opt);
  FrameCanvas *offscreen_canvas = matrix->CreateFrameCanvas();

  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  std::deque<std::pair<int, int>> Snaketest;
  snake.push_back(std::make_pair(10, 10)); // Initial position of the snake
  int food_x = 15, food_y = 15; // Initial position of the food

  int direction = 'l'; // Use 'w', 'a', 's', 'd' for up, left, down, right
  while (!interrupt_received) {
    if (kbhit()) {
      direction = tolower(getchar());
    }

    // Calculate new head position based on the direction
    std::pair<int, int> head = snake.front();
    switch (direction) {
      case 'd': head.first--; break;
      case 'a': head.first++; break;
    }               

    // Check if snake eats the food
    if (head.first == food_x && head.second == food_y) {
      // Place new food at random
      food_x = random() % matrix->width();
      food_y = random() % matrix->height();
      // No pop_back if snake eats the food it grows
    } else {
      snake.pop_back(); // Move the snake
    }

    // Put the new head at the front of the deque
    snake.push_front(head);

    // Clear the display
    offscreen_canvas->Clear();
    // Draw food
    offscreen_canvas->SetPixel(food_x, food_y, 255, 0, 0);

    // Draw snake
    for (const auto &pos : snake) {
      offscreen_canvas->SetPixel(pos.first, pos.second, 0, 255, 0);
    }

    // Swap on VSync and sleep a little
    offscreen_canvas = matrix->SwapOnVSync(offscreen_canvas);
    usleep(200000);
  }

  delete matrix;
  return 0;
}



#include "led-matrix.h"
#include "graphics.h"
#include <unistd.h>
#include <termios.h>
#include <deque>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <fcntl.h>
#include <algorithm>

using namespace std;
using namespace rgb_matrix;

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}


char getCommand() {
    std::ifstream cmdFile("/tmp/snake_commands");
    char cmd = '\0';
    if (cmdFile.is_open()) {
        cmdFile >> cmd;
        cmdFile.close();

        
        std::ofstream ofs;
        ofs.open("/tmp/snake_commands", std::ofstream::out | std::ofstream::trunc);
        ofs.close();
    }
    return cmd;
}

void resetGame(RGBMatrix *matrix, deque<pair<int, int>>& snake, int &food_x, int &food_y) {
  snake.clear();
  snake.push_back(make_pair(matrix->width() / 2, matrix->height() / 2)); 
  food_x = rand() % matrix->width();
  food_y = rand() % matrix->height();
}

int main(int argc, char *argv[]) {
  RGBMatrix::Options matrix_options;
  rgb_matrix::RuntimeOptions runtime_opt;

  matrix_options.rows = 64;
  matrix_options.cols = 64;
  matrix_options.chain_length = 1;
  matrix_options.parallel = 1;
  matrix_options.hardware_mapping = "regular";

  RGBMatrix *matrix = RGBMatrix::CreateFromOptions(matrix_options, runtime_opt);
  FrameCanvas *offscreen_canvas = matrix->CreateFrameCanvas();

  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  deque<pair<int, int>> snake;
  snake.push_back(make_pair(10, 10)); 
  int food_x = 15, food_y = 15;
  int direction = 'l'; 
  int last_direction = direction; 

  while (!interrupt_received) {
    char new_direction = getCommand();
    if (new_direction != '\0') {
      switch (new_direction) {
        case 'w': if (last_direction != 's') direction = 'n'; break;
        case 's': if (last_direction != 'n') direction = 's'; break;
        case 'a': if (last_direction != 'e') direction = 'w'; break;
        case 'd': if (last_direction != 'w') direction = 'e'; break;
      }
      last_direction = direction;
    }

    pair<int, int> head = snake.front();
    switch (direction) {
      case 'n': head.second--; break;
      case 's': head.second++; break;
      case 'w': head.first--; break;
      case 'e': head.first++; break;
    }

    if (head.first < 0 || head.first >= matrix->width() || 
        head.second < 0 || head.second >= matrix->height() || 
        find(snake.begin(), snake.end(), head) != snake.end()) {
      resetGame(matrix, snake, food_x, food_y);
      continue;
    }

    if (head.first == food_x && head.second == food_y) {
      food_x = rand() % matrix->width();
      food_y = rand() % matrix->height();
    } else {
      snake.pop_back();
    }

    snake.push_front(head);

    offscreen_canvas->Clear();
    offscreen_canvas->SetPixel(food_x, food_y, 255, 0, 0); 
    for (const auto &pos : snake) {
      offscreen_canvas->SetPixel(pos.first, pos.second, 0, 255, 0); 
    }

    offscreen_canvas = matrix->SwapOnVSync(offscreen_canvas);
    usleep(200000);
  }

  delete matrix;
  return 0;
}
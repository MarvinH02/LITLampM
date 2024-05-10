#include "led-matrix.h"
#include "graphics.h"
#include <unistd.h>
#include <termios.h>
#include <deque>
#include <signal.h>
#include <fcntl.h>
#include <algorithm> 


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

void resetGame(RGBMatrix *matrix, deque<pair<int, int>> &snake, int &food_x, int &food_y) {
  snake.clear();
  snake.push_back(make_pair(matrix->width() / 2, matrix->height() / 2));  // Reset snake to center
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
  snake.push_back(make_pair(10, 10)); // Initial position of the snake
  int food_x = 15, food_y = 15; // Initial position of the food

  int direction = 'l'; // Use 'w', 'a', 's', 'd' for up, left, down, right
  int last_direction = direction; // to prevent U-turns
  while (!interrupt_received) {
    if (kbhit()) {
      char new_direction = tolower(getchar());
      // Prevent reverse direction
      if ((new_direction == 'w' && last_direction != 's') ||
          (new_direction == 's' && last_direction != 'w') ||
          (new_direction == 'a' && last_direction != 'd') ||
          (new_direction == 'd' && last_direction != 'a')) {
        direction = new_direction;
        last_direction = direction; // Update last direction after a valid change
      }
    }

    // Calculate new head position based on the direction
    pair<int, int> head = snake.front();
    switch (direction) {
      case 'w': head.second--; break;
      case 's': head.second++; break;
      case 'a': head.first--; break;
      case 'd': head.first++; break;
    }

    // Check for boundary collision or self-collision
    if (head.first < 0 || head.first >= matrix->width() || 
        head.second < 0 || head.second >= matrix->height() || 
        find(snake.begin(), snake.end(), head) != snake.end()) { // Check self-collision
      resetGame(matrix, snake, food_x, food_y);
      continue;
    }

    // Check if snake eats the food
    if (head.first == food_x && head.second == food_y) {
      food_x = rand() % matrix->width();
      food_y = rand() % matrix->height();
    } else {
      snake.pop_back(); // Move the snake
    }

    snake.push_front(head); // Update the snake's position

    offscreen_canvas->Clear();
    offscreen_canvas->SetPixel(food_x, food_y, 255, 0, 0); // Draw food

    for (const auto &pos : snake) {
      offscreen_canvas->SetPixel(pos.first, pos.second, 0, 255, 0); // Draw snake
    }

    offscreen_canvas = matrix->SwapOnVSync(offscreen_canvas);
    usleep(100000); // Control the speed of the snake
  }

  delete matrix;
  return 0;
}

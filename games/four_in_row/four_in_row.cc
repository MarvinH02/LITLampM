#include "/home/g10/rpi-rgb-led-matrix/include/led-matrix.h"
#include "/home/g10/rpi-rgb-led-matrix/include/graphics.h"
#include <unistd.h>
#include <termios.h>
#include <iostream>
#include <vector>

using namespace rgb_matrix;
using namespace std;

const int kRows = 6;
const int kColumns = 7;
volatile bool interrupt_received = false;

struct Termios {
    struct termios orig_termios;
    static void disableRawMode() {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    }
    void enableRawMode() {
        tcgetattr(STDIN_FILENO, &orig_termios);
        atexit(disableRawMode);
        struct termios raw = orig_termios;
        raw.c_lflag &= ~(ECHO | ICANON);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    }
} term;

struct Game {
    int board[kRows][kColumns] = {{0}};
    int currentPlayer = 1;

    void addPiece(int col) {
        for (int i = kRows - 1; i >= 0; --i) {
            if (board[i][col] == 0) {
                board[i][col] = currentPlayer;
                break;
            }
        }
    }

    bool checkWin() {
        // Horizontal, vertical, and diagonal checks
        for (int row = 0; row < kRows; ++row) {
            for (int col = 0; col < kColumns; ++col) {
                if (col + 3 < kColumns &&
                    board[row][col] == currentPlayer &&
                    board[row][col + 1] == currentPlayer &&
                    board[row][col + 2] == currentPlayer &&
                    board[row][col + 3] == currentPlayer) return true;

                if (row + 3 < kRows &&
                    board[row][col] == currentPlayer &&
                    board[row + 1][col] == currentPlayer &&
                    board[row + 2][col] == currentPlayer &&
                    board[row + 3][col] == currentPlayer) return true;

                if (row + 3 < kRows && col + 3 < kColumns &&
                    board[row][col] == currentPlayer &&
                    board[row + 1][col + 1] == currentPlayer &&
                    board[row + 2][col + 2] == currentPlayer &&
                    board[row + 3][col + 3] == currentPlayer) return true;

                if (row - 3 >= 0 && col + 3 < kColumns &&
                    board[row][col] == currentPlayer &&
                    board[row - 1][col + 1] == currentPlayer &&
                    board[row - 2][col + 2] == currentPlayer &&
                    board[row - 3][col + 3] == currentPlayer) return true;
            }
        }
        return false;
    }

    void drawBoard(FrameCanvas *canvas) {
        canvas->Clear();
        for (int row = 0; row < kRows; ++row) {
            for (int col = 0; col < kColumns; ++col) {
                if (board[row][col] == 1)
                    DrawCircle(canvas, col * 10 + 5, row * 10 + 5, 4, Color(255, 0, 0));
                else if (board[row][col] == 2)
                    DrawCircle(canvas, col * 10 + 5, row * 10 + 5, 4, Color(0, 0, 255));
            }
        }
    }
};

int main(int argc, char *argv[]) {
    RGBMatrix::Options matrix_options;
    matrix_options.rows = 64;
    matrix_options.cols = 64;
    matrix_options.chain_length = 1;
    matrix_options.parallel = 1;
    matrix_options.hardware_mapping = "regular";

    RGBMatrix *matrix = RGBMatrix::CreateFromOptions(matrix_options, RuntimeOptions());
    if (matrix == NULL) return 1;

    FrameCanvas *canvas = matrix->CreateFrameCanvas();

    Game game;
    term.enableRawMode();
    while (!interrupt_received) {
        game.drawBoard(canvas);
        canvas = matrix->SwapOnVSync(canvas);

        if (cin.peek() != EOF) {
            char c = cin.get();
            if (c >= '1' && c <= '7') {
                game.addPiece(c - '1');
                if (game.checkWin()) {
                    cout << "Player " << game.currentPlayer << " wins!\n";
                    interrupt_received = true;
                }
                game.currentPlayer = 3 - game.currentPlayer;
            } else if (c == 'q' || c == 'Q') {
                interrupt_received = true;
            }
        }
        usleep(100000);  // Delay to slow down the game loop for visibility
    }

    delete matrix;  // Clean up
    return 0;
}

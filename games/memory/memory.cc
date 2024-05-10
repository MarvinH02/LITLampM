#include <iostream>
#include <vector>
#include </home/g10/rpi-rgb-led-matrix/include/graphics.h> // Include the header file for the DrawText function
#include </home/g10/rpi-rgb-led-matrix/include/pixel-mapper.h>
#include <assert.h>
#include <getopt.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <algorithm>
#include </home/g10/rpi-rgb-led-matrix/include/led-matrix.h>

void displayNumbers(const std::vector<int>& numbers, rgb_matrix::RGBMatrix* matrix) {
    // Display numbers on the LED matrix
    for (int num : numbers) {
        // Convert the number to a string
        std::string numStr = std::to_string(num);
        
        // Display each digit of the number on the LED matrix
        for (char digit : numStr) {
            // Convert the digit to an integer
            int digitInt = digit - '0';
            
            // Display the digit on the LED matrix
            matrix->Clear();
            matrix->DrawText(0, 0, rgb_matrix::Color(255, 255, 255), "Digit: " + std::to_string(digitInt));
            matrix->Update();
            
            // Wait for a short duration before displaying the next digit
            usleep(1000000); // Adjust the delay as needed
        }
    }
}

bool checkGuess(const std::vector<int>& numbers, const std::vector<int>& guess) {
    // Check if the guess matches the numbers
    for (int i = 0; i < numbers.size(); i++) {
        if (numbers[i] != guess[i]) {
            return false;
        }
    }
    
    return true;
}

int main() {
    int n;
    std::cout << "Enter the number of random numbers to display: ";
    std::cin >> n;
    
    std::vector<int> numbers(n);
    std::vector<int> guess(n);
    
    // Generate random numbers
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1, 100);
    
    for (int i = 0; i < n; i++) {
        numbers[i] = dis(gen);
    }
    
    // Initialize the LED matrix
    rgb_matrix::RGBMatrix::Options matrixOptions;
    matrixOptions.hardware_mapping = "regular"; // Adjust this based on your setup
    matrixOptions.rows = 32; // Adjust this based on your LED matrix size
    matrixOptions.cols = 64; // Adjust this based on your LED matrix size
    rgb_matrix::RGBMatrix* matrix = rgb_matrix::CreateMatrixFromOptions(matrixOptions);
    
    // Display the numbers on the LED matrix
    displayNumbers(numbers, matrix);
    
    // Wait for the user to memorize the numbers
    std::cout << "Press ENTER to continue...";
    std::cin.ignore();
    std::cin.get();

    // Clear the console
    std::cout << "\033[2J\033[1;1H";

    // Get user's guess
    std::cout << "Enter your guess for each number: ";
    for (int i = 0; i < n; i++) {
        std::cin >> guess[i];
    }
    
    // Check the guess
    if (checkGuess(numbers, guess)) {
        std::cout << "Congratulations! You guessed all the numbers correctly." << std::endl;
    } else {
        std::cout << "Oops! Your guess was incorrect." << std::endl;
    }
    
    // Clean up the LED matrix
    delete matrix;
    
    return 0;
}
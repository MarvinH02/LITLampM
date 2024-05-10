#include "led-matrix-c.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
  struct RGBLedMatrixOptions options;
  struct RGBLedMatrix *matrix;
  struct LedCanvas *offscreen_canvas;
  int width, height;
  int x, y;

  memset(&options, 0, sizeof(options));
  options.rows = 32;
  options.chain_length = 1;

  matrix = led_matrix_create_from_options(&options, &argc, &argv);
  if (matrix == NULL)
    return 1;

  offscreen_canvas = led_matrix_create_offscreen_canvas(matrix);

  led_canvas_get_size(offscreen_canvas, &width, &height);

  fprintf(stderr, "Size: %dx%d. Hardware gpio mapping: %s\n",
          width, height, options.hardware_mapping);

  // Set all pixels to red color
  for (y = 0; y < height; ++y) {
    for (x = 0; x < width; ++x) {
      led_canvas_set_pixel(offscreen_canvas, x, y, 0xff, 0, 0); // Set only the red component
    }
  }

  // Swap the canvas to display the red color
  offscreen_canvas = led_matrix_swap_on_vsync(matrix, offscreen_canvas);

  // Wait indefinitely
  pause();

  /*
   * Make sure to always call led_matrix_delete() in the end to reset the
   * display. Installing signal handlers for defined exit is a good idea.
   */
  led_matrix_delete(matrix);

  return 0;
}

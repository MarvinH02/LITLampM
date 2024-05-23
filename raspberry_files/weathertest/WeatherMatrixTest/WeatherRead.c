#include "led-matrix-c.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct RGBLedMatrix *matrix;
struct RGBLedMatrixOptions options;
memset(&options, 0, sifeof(options))
options.rows = 64;
options.chain_length = 1;
matrix = led_matrix_create_from_options(&options, &argc, &argv);

File *file = fopen("sensor_data.txt", "r");
char buffer[256];
while (fgets(buffer, sizeof(buffer), file)){
	
}

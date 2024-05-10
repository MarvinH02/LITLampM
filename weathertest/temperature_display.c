#include <stdio.h>
#include <bcm2835.h>
#include <bme280.h>

#define LED_WIDTH 64
#define LED_HEIGHT 64


int main(int argc, char **argv) {
   if (!bcm2835_init()) {
	   printf("bcm2835_failed");
	   return 1;
   }
   
   if (!bme280_init(1, 0x76)) {
	   printf("bme280_failed\n");
	   return 1;
   }
   
   while (1) {
	   double temperature;
	   if(bme280_read_temperature(&temperature)!=BME280_OK) {
		   printf("failed to read temp"\n);
		   continue; 
	   }
	   
	   printf("Temperature: %.2fC\n", temperature);
	   
	   bcm2835_delay(1000);}
	   
	   bcm2835_close();
	   return 0; } 

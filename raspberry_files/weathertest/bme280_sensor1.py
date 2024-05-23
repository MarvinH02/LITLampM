import time
import bme280
import smbus2
import sys
from rgbmatrix import RGBMatrix, RGBMatrixOptions

def main ():
    
    options = RGBMatrixOptions()
    options.rows = 32
    options.cols = 64
    options.chain_length = 1
    options.parallel = 1
    options.hardware_mapping = 'regular'
    matrix = RGBMatrix(options = options)

port = 1
address = 0x76
bus = smbus2.SMBus(port)
cal_data = bme280.load_calibration_params(bus,address)

while True:
    data = bme280.sample(bus,address, cal_data)
    temperature = data.temperature
    
    matrix.Clear()
    matrix.DrawText(0,16,(255, 255, 255), str(round(temperature, 2))
    
    time.sleep(1)
    
    if __name__== "__main__":
        main()

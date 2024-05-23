#!/usr/bin/env python3

import smbus2
import bme280

print(bme280.__file__)

from time import sleep

port = 1
address = 0x76
bus = smbus2.SMBus(port)

with open("sensor_data.txt", "w") as file:
    pass

while True:
    with open("sensor_data.txt", "a") as file:
        bme280_data = bme280.sample(bus, address)
        humidity = bme280_data.humidity
        pressure = bme280_data.pressure
        ambient_temperature = bme280_data.temperature

        data_str = f"Humidity: {humidity:.2f}%, Pressure: {pressure:.2f} hPa, Temp: {ambient_temperature:.2f} C\n"

        file.write(data_str)

        file.flush()

    sleep(1)
    
    
    
    #!/usr/bin/env python3

import smbus2
import bme280
from time import sleep

port = 1
address = 0x76
bus = smbus2.SMBus(port)

bme280.load_calibration_params(bus, address)
with open("sensor_data.txt", "w") as file:
    pass

while True:
    with open("sensor_data.txt", "a") as file:
        bme280_data = bme280.sample(bus, address)
        humidity = bme280_data.humidity
        pressure = bme280_data.pressure
        ambient_temperature = bme280_data.temperature

        data_str = f"Humidity: {humidity:.2f}%, Pressure: {pressure:.2f} hPa, Temp: {ambient_temperature:.2f} C\n"

        file.write(data_str)

        file.flush()

    sleep(1)
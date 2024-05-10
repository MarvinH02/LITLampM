import bme280
import smbus2
from time import sleep

port = 1
address = 0x76  
bus = smbus2.SMBus(port)

bme280.load_calibration_params(bus, address)
counter =0
while True:

    with open("sensor_data.txt", "a" if counter < 10 else "w") as file:
        if counter == 10:
            counter = 0

        bme280_data = bme280.sample(bus, address)
        humidity = bme280_data.humidity
        pressure = bme280_data.pressure
        ambient_temperature = bme280_data.temperature

        data_str = f"Humidity: {humidity:.2f}%, Pressure: {pressure:.2f} hPa, Temperature: {ambient_temperature:.2f} C\n"
        
        file.write(data_str)
        
        file.flush()
        counter +=1
        
    sleep(1)

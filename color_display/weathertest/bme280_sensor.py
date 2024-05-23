#!/usr/bin/env python3

import smbus2
from time import sleep

port = 1
address = 0x76
bus = smbus2.SMBus(port)

def read_calibration_data(bus, address):
    calibration = {}

    calibration['dig_T1'] = bus.read_word_data(address, 0x88)
    calibration['dig_T2'] = bus.read_word_data(address, 0x8A)
    calibration['dig_T3'] = bus.read_word_data(address, 0x8C)

    calibration['dig_P1'] = bus.read_word_data(address, 0x8E)
    calibration['dig_P2'] = bus.read_word_data(address, 0x90)
    calibration['dig_P3'] = bus.read_word_data(address, 0x92)
    calibration['dig_P4'] = bus.read_word_data(address, 0x94)
    calibration['dig_P5'] = bus.read_word_data(address, 0x96)
    calibration['dig_P6'] = bus.read_word_data(address, 0x98)
    calibration['dig_P7'] = bus.read_word_data(address, 0x9A)
    calibration['dig_P8'] = bus.read_word_data(address, 0x9C)
    calibration['dig_P9'] = bus.read_word_data(address, 0x9E)

    calibration['dig_H1'] = bus.read_byte_data(address, 0xA1)
    calibration['dig_H2'] = bus.read_word_data(address, 0xE1)
    calibration['dig_H3'] = bus.read_byte_data(address, 0xE3)
    calibration['dig_H4'] = (bus.read_byte_data(address, 0xE4) << 4) | (bus.read_byte_data(address, 0xE5) & 0x0F)
    calibration['dig_H5'] = (bus.read_byte_data(address, 0xE6) << 4) | (bus.read_byte_data(address, 0xE5) >> 4)
    calibration['dig_H6'] = bus.read_byte_data(address, 0xE7)

    return calibration

def compensate_temperature(adc_T, calibration):
    var1 = (((adc_T >> 3) - (calibration['dig_T1'] << 1)) * calibration['dig_T2']) >> 11
    var2 = (((((adc_T >> 4) - calibration['dig_T1']) * ((adc_T >> 4) - calibration['dig_T1'])) >> 12) * calibration['dig_T3']) >> 14
    t_fine = var1 + var2
    temperature = (t_fine * 5 + 128) >> 8
    return temperature / 100.0, t_fine

def compensate_pressure(adc_P, t_fine, calibration):
    var1 = t_fine - 128000
    var2 = var1 * var1 * calibration['dig_P6']
    var2 = var2 + ((var1 * calibration['dig_P5']) << 17)
    var2 = var2 + (calibration['dig_P4'] << 35)
    var1 = ((var1 * var1 * calibration['dig_P3']) >> 8) + ((var1 * calibration['dig_P2']) << 12)
    var1 = (((1 << 47) + var1) * calibration['dig_P1']) >> 33

    if var1 == 0:
        return 0

    pressure = 1048576 - adc_P
    pressure = ((pressure << 31) - var2) * 3125 // var1
    var1 = (calibration['dig_P9'] * (pressure >> 13) * (pressure >> 13)) >> 25
    var2 = (calibration['dig_P8'] * pressure) >> 19

    pressure = ((pressure + var1 + var2) >> 8) + (calibration['dig_P7'] << 4)
    return pressure / 256.0 / 100.0

def compensate_humidity(adc_H, t_fine, calibration):
    var_h = t_fine - 76800
    var_h = (((((adc_H << 14) - (calibration['dig_H4'] << 20) - (calibration['dig_H5'] * var_h)) + 16384) >> 15) *
             (((((((var_h * calibration['dig_H6']) >> 10) * (((var_h * calibration['dig_H3']) >> 11) + 32768)) >> 10) + 2097152) *
               calibration['dig_H2'] + 8192) >> 14))
    var_h = var_h - (((((var_h >> 15) * (var_h >> 15)) >> 7) * calibration['dig_H1']) >> 4)
    var_h = max(var_h, 0)
    var_h = min(var_h, 419430400)
    return (var_h >> 12) / 1024.0

calibration_params = read_calibration_data(bus, address)

def read_bme280_data(bus, address, calibration_params):
    
    bus.write_byte_data(address, 0xF2, 0x01)
    bus.write_byte_data(address, 0xF4, 0x27)
    bus.write_byte_data(address, 0xF5, 0xA0)

    sleep(0.01)

    data = bus.read_i2c_block_data(address, 0xF7, 8)
    raw_pressure = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4)
    raw_temp = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4)
    raw_humidity = (data[6] << 8) | data[7]

    temperature, t_fine = compensate_temperature(raw_temp, calibration_params)
    pressure = compensate_pressure(raw_pressure, t_fine, calibration_params)
    humidity = compensate_humidity(raw_humidity, t_fine, calibration_params)

    return humidity, pressure, temperature

while True:
    with open("sensor_data.txt", "w") as file:
        humidity, pressure, temperature = read_bme280_data(bus, address, calibration_params)

        data_str = (
            f"Humid: {humidity:.2f},"
            f" Press: {pressure:.2f} hPa,"
            f" Temp: {temperature:.2f}C"
        )
        file.write(data_str)
        file.flush()

    sleep(1)
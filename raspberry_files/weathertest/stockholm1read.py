import requests
import math
import sys  # Import sys to access command line arguments
from time import sleep

api_key = "6cc8c20d3bea4394060c84ca23fd1175"

def get_weather(api_key, city):
    url = f"http://api.openweathermap.org/data/2.5/weather?q={city}&appid={api_key}"
    response = requests.get(url).json()

    city_name = response['name']
    temp = response['main']['temp']
    temp = math.floor(temp - 273.15)  # Convert Kelvin to Celsius
    weather = response['weather'][0]['description']

    return city_name, temp, weather

if len(sys.argv) > 1:  # Check if a city name is provided
    city_name = sys.argv[1]  # Use the provided city name
else:
    city_name = "Stockholm"  # Default city

while True:
    with open("weather_data.txt", "w") as file:
        city_name, temp, weather = get_weather(api_key, city_name)
        data_str = f"City: {city_name}\nTemperature: {temp}°C\nWeather: {weather}\n\n"
        file.write(data_str)
        file.flush()

    sleep(10)

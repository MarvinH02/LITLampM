import requests
import math
from time import sleep

city_name = "miami"
api_key = "6cc8c20d3bea4394060c84ca23fd1175"  # Replace with your actual API key

def get_weather(api_key, city):
    url = f"http://api.openweathermap.org/data/2.5/weather?q={city}&appid={api_key}"
    response = requests.get(url).json()
    
    city_name = response['name']
    temp = response['main']['temp']
    temp = math.floor(temp - 273.15)  # Convert Kelvin to Celsius
    weather = response['weather'][0]['description']
    
    return city_name, temp, weather

counter = 0
while True:
    mode = "a" if counter < 10 else "w"
    with open("weather_data.txt", mode) as file:
        if counter == 10:
            counter = 0

        city_name, temp, weather = get_weather(api_key, city_name)
        data_str = f"City: {city_name}\nTemperature: {temp}°C\nWeather: {weather}\n\n"
        
        file.write(data_str)
        
        file.flush()
        counter += 1
        
    sleep(1000)

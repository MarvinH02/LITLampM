import requests  
import math
from tkinter import *  # Note: Tkinter is imported but not used in this snippet

city_name = "stockholm"
api_key = "your_api_key_here"  # Replace with your actual API key

def get_weather(api_key, city):
    url = f"http://api.openweathermap.org/data/2.5/weather?q={city}&appid={api_key}"
    response = requests.get(url).json()
    
    city_name = response['name']
    temp = response['main']['temp']
    temp = math.floor(temp - 273.15)  # Convert Kelvin to Celsius
    weather = response['weather'][0]['description']
    
    # Write the data to a file
    with open("weather_data.txt", "w") as file:
        file.write(f"City: {city_name}\n")
        file.write(f"Temperature: {temp}°C\n")
        file.write(f"Weather: {weather}\n")

get_weather(api_key, city_name)

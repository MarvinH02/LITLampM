import requests  
from tkinter import *
import math

city_name = "Stockholm"
api_key = "6cc8c20d3bea4394060c84ca23fd1175"

def get_weather(api_key, city):
	url = f"http://api.openweathermap.org/data/2.5/weather?q={city}&appid={api_key}"

	response = requests.get(url).json()
	
    # city name
	city_name = response['name']
	print(city_name)
	
    # temp
	temp = response['main']['temp']
	temp = math.floor(temp - 273.15) 
	print(temp)
	
    # clear sky 
	weather = response['weather'][0]['description']
	print(weather)
	

get_weather(api_key, city_name)


import requests
import json

api_key = "fdabca3721f79a683b340814ab938c57"
lon = "126.9780"
lat = "37.5665"

url = "https://api.openweathermap.org/data/2.5/onecall?lat=%s&lon=%s&exclude=current,minutely,hourly,alerts&appid=%s&units=metric" % (lat, lon, api_key)
response = requests.get(url)
data = json.loads(response.text)
print(data)
print(url)
#print(json.dumps(data["daily"], sort_keys=True, indent=4))
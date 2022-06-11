import requests
import json

api_key = "fdabca3721f79a683b340814ab938c57"
lat = "48.20"
lon = "16.38"

url = "https://api.openweathermap.org/data/2.5/onecall?lat=%s&lon=%s&exclude=current,minutely,hourly,alerts&appid=%s&units=metric" % (lat, lon, api_key)
response = requests.get(url)
data = json.loads(response.text)
print(data)
#print(json.dumps(data["daily"], sort_keys=True, indent=4))
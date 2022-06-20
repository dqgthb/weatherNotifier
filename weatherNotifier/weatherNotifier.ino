#include <SoftwareSerial.h>
#define espSerial Serial
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 13

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 8

SoftwareSerial esp8266(2, 3); // Arduino TX (ESP8266 RX) connected to Arduino Pin 2, Arduino RX(ESP8266 TX) connected to Arduino Pin 3

String ssid = "arstarst";
String password = "44332211";

// String api = "/index.html";
String api = "/data/2.5/onecall?lat=37.5&lon=126.9&exclude=current,minutely,hourly,alerts&appid=fdabca3721f79a683b340814ab938c57&units=metric";

// String server = "naver.com";
String server = "api.openweathermap.org";
String fullUrl = server + api;
String fullLen = String(fullUrl.length());
String request = "GET " + api + " HTTP/1.1\r\n" + "Host: " + server + "\r\n" +
                 "Accept: application/json\r\n" +
                 "Content-Type: application/json\r\n" +
                 "Connection: keep-alive\r\n\r\n";
String requestLength = String(request.length());
String data;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

double pop; // probability of precipitation

int delayval = 500; // delay for half a second

int PIR = 7;   // 센서 변수 선언하고 연결한 핀 번호로 초기화
int state = 0; // 센서 상태 값 저장 변수(0: LOW, 1: HIGH)

const int timeout = 2000;

void setup()
{
  Serial.begin(9600);
  esp8266.begin(9600); // Should match ESP's current baudrate

  // setupESP8266();
  // connectToWiFi();

  startTCPConnection();
  data = sendGetRequest();

  // atCommand("AT_HTTPURLCFG=" + fullLen, timeout);
  // atCommand(fullUrl , 200000);
  // atCommand("AT+HTTPCGET=\"\"", timeout);
  closeTCPConnection();


#if defined(__AVR_ATtiny85__)
  if (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  pixels.begin();      // This initializes the NeoPixel library.
  pinMode(PIR, INPUT); // 센서를 입력으로 설정
  Serial.begin(9600);  // 시리얼 통신, 속도는 9600

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, data);

  if (error)
  {
    Serial.print(F("deserialization failed: "));
    Serial.println(error.f_str());
    return;
  }
  pop = doc["daily"][0]["pop"];
  Serial.println(pop);
}


String atCommand(String command, int timeout)
{
  String response = "";
  esp8266.println(command);

  long int time = millis();

  while ((time + timeout) > millis())
  {
    while (esp8266.available())
    {
      char c = esp8266.read();
      response += c;
    }
  }

  Serial.println(response);
  return response;
}

void setupESP8266()
{
  atCommand("AT+UART_DEF=9600,8,1,0,0", timeout);
  atCommand("AT+RST", timeout);
  atCommand("AT+CWMODE=1", timeout);
}

void connectToWiFi()
{
  String connect = "AT+CWJAP=\"" + ssid + "\",\"" + password + "\"";
  atCommand(connect, 6000);
  atCommand("AT+CIFSR", timeout);
}

void startTCPConnection()
{
  String connect = "AT+CIPSTART=\"TCP\",\"" + server + "\",80";
  atCommand(connect, timeout);
}

void closeTCPConnection()
{
  atCommand("AT+CIPCLOSE", timeout);
}

String sendGetRequest()
{
  atCommand("AT+CIPSEND=" + requestLength, timeout);
  String response = atCommand(request, 200000);
  return response;
}

void updatePop(){
  startTCPConnection();
  data = sendGetRequest();
  closeTCPConnection();
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, data);

  if (error)
  {
    Serial.print(F("deserialization failed: "));
    Serial.println(error.f_str());
    return;
  }
  pop = doc["daily"][0]["pop"];
  Serial.println(pop);

}

void loop()
{
  state = digitalRead(PIR); // PIR 센서값 입력받음
                            // 초기 LED값을 0FF로 설정해주기

  if (state == 1)
  {                     // 센서 값이 1 이라면
    updatePop(); // pop update
    Serial.print("ON"); // 시리얼 통신에 센서값 출력

    // pop: precipitation probability, 강수확률에 따라 LED 색상 변경
    if (pop < 0.1)
    { // 강수확률이 10% 보다 낮을 때 LED를 파란색으로 켜기
      for (int i = 0; i < NUMPIXELS; i++)
      {
        pixels.setPixelColor(i, pixels.Color(0, 0, 255));
      }
      pixels.show();
    }
    if else (pop < 0.3){
      for (int i = 0; i < NUMPIXELS; i++)
      {
        pixels.setPixelColor(i, pixels.Color(0, 255, 0));
      }
      pixels.show();
    }
    if else (pop < 0.5){
      for (int i = 0; i < NUMPIXELS; i++)
      {
        pixels.setPixelColor(i, pixels.Color(255, 255, 0));
      }
      pixels.show();
    }
    if else (pop < 0.8){
      for (int i = 0; i < NUMPIXELS; i++)
      {
        pixels.setPixelColor(i, pixels.Color(255, 0, 255));
      }
      pixels.show();
    }
    else
    {
      for (int i = 0; i < NUMPIXELS; i++)
      {
        pixels.setPixelColor(i, pixels.Color(255, 0, 0));
      }
      pixels.show();
    }
    // LED ON
    delay(10000);
  }

  else
  {                      // 센서 값이 0 이라면
    Serial.print("OFF"); // 시리얼 모니터에 LED 상태 출력
    for (int i = 0; i < NUMPIXELS; i++)
    {

      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // Moderately bright green color.

      pixels.show(); // This sends the updated pixel color to the hardware.

      delay(delayval); // Delay for a period of time (in milliseconds).

    } // LED OFF
    delay(1);
  }
  pixels.begin();
  delay(1); // 0.1초 대기(1000ms = 1s)
}

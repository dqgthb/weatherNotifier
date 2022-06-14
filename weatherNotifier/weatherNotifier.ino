#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN            13

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      8


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

double pop; // probability of precipitation

int delayval = 500; // delay for half a second

int PIR = 7;        // 센서 변수 선언하고 연결한 핀 번호로 초기화
int state = 0;      // 센서 상태 값 저장 변수(0: LOW, 1: HIGH)
 
void setup() {
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  pixels.begin(); // This initializes the NeoPixel library.
  pinMode(PIR, INPUT);      // 센서를 입력으로 설정
  Serial.begin(9600);      // 시리얼 통신, 속도는 9600

  StaticJsonDocument<200> doc;
  char json[] = "{\"daily\": [{\"pop\":0.1}]}";

  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    Serial.print(F("deserialization failed: "));
    Serial.println(error.f_str());
    return;
  }
  pop = doc["daily"][0]["pop"];
  Serial.println(pop);
}
 
void loop() {
  state = digitalRead(PIR);   // PIR 센서값 입력받음
         // 초기 LED값을 0FF로 설정해주기
  
  if (state == 1){              // 센서 값이 1 이라면
    Serial.print("ON");       // 시리얼 통신에 센서값 출력

    // pop: precipitation probability, 강수확률에 따라 LED 색상 변경
    if (pop < 0.3) { // 강수확률이 50% 보다 낮을 때 LED를 파란색으로 켜기
      for(int i=0; i<NUMPIXELS; i++){
        pixels.setPixelColor(i, pixels.Color(0,0,255));
      }
      pixels.show();
    } else { // 강수확률이 50~100% 일 경우 LED을 빨간색으로 켜기
      for(int i=0; i<NUMPIXELS; i++){
        pixels.setPixelColor(i, pixels.Color(255,0,0));
      }
      pixels.show();
    }
     // LED ON
    delay(10000);
  }
  
  else {                        // 센서 값이 0 이라면
    Serial.print("OFF");      // 시리얼 모니터에 LED 상태 출력
    for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(0,0,0)); // Moderately bright green color.

    pixels.show(); // This sends the updated pixel color to the hardware.

    delay(delayval); // Delay for a period of time (in milliseconds).

  }     // LED OFF
    delay(1);
  }
  pixels.begin();
  delay(1);   // 0.1초 대기(1000ms = 1s)
 
}

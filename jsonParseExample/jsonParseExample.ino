#define LED 13 // The pin the LED is connected to
#include <ArduinoJson.h>

// HOW TO PARSE JSON

double pop; // probability of precipitation

void setup()
{
  Serial.begin(9600);

  pinMode(LED, OUTPUT); // Declare the LED as an output

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

void loop()
{
  digitalWrite(LED, HIGH); // Turn the LED on
  delay(1000);
  digitalWrite(LED, LOW);
  delay(1000);
  Serial.println(pop);
}

#include <SoftwareSerial.h>

int val;

void setup() {
  Serial.begin(57600);
}

void loop() {
    if (Serial.available()) {
      val = Serial.parseInt(); //see whatever came in on Serial.read()
      Serial.println(val);
    }
}

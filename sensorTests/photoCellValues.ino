void setup() {
Serial.begin(57600);
}

void loop() {
  Serial.println(analogRead(3));

}

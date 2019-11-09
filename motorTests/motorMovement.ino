#include <SoftwareSerial.h>
#include <PololuQik.h>

PololuQik2s9v1 qikF(2, 3, 4);
PololuQik2s9v1 qikB(10,11,12);

void go(float degree)
{
   float pi = 3.1415;
   float rad = degree/(180/pi); 
   int FLSpeed = (-(sin(rad)*127)*(sqrt(3)/2)) - ((cos(rad)*127)/2); // front-left
   int FRSpeed  = (sin(rad)*127)*sqrt(3)/2 - (cos(rad)*127)/2; // front-right
   int BSpeed = (cos(rad)*127);

   qikF.setM0Speed(FLSpeed);
   qikF.setM1Speed(FRSpeed);
   qikB.setM1Speed(BSpeed);
}

void stop()
{
   qikF.setM0Speed(0);
   qikF.setM1Speed(0);
   qikB.setM0Speed(0);
}
void setup() {
  qikF.init();
  qikB.init();
  go(90);
  delay(1000);
  stop();
  delay(1000);
  go(270);
  delay(1000);
  stop();
  delay(1000);
  go(0);
  delay(1000);
  stop();
  delay(1000);
  go(180);
  delay(1000);
  stop();
  delay(1000);
}

void loop() {
}

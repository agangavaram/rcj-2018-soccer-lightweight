#include <SoftwareSerial.h>
#include <PololuQik.h>

//tx,rx,reset
PololuQik2s12v10 qikF(2, 3, 4);
PololuQik2s12v10 qikB(10, 11, 12);

int targ;
int FLSpeed;
int FRSpeed;
int BSpeed;
bool goingL, goingR, goingB, goingS = true;
int photocellPin = 3;
int photocellReading;

void go(float degree)
{
  Serial.println(degree);
  static int speed = 80;
  static float pi = 3.1415;
  float rad = degree / (180 / pi);

  FLSpeed = (sin(rad) * speed) * (sqrt(3) / 2) - ((cos(rad) * speed) / 2); // front-left
  FRSpeed  = (-(sin(rad) * speed) * (sqrt(3) / 2)) - (cos(rad) * speed) / 2; // front-right
  BSpeed = (cos(rad) * speed);
  Serial.print("Speeds (L,R,B): ");
  Serial.print(FLSpeed);
  Serial.print(" ");
  Serial.println(FRSpeed);
  Serial.print(" ");
  Serial.println(BSpeed);
  

  qikF.setM1Speed(FLSpeed);
  qikF.setM0Speed(FRSpeed);
  qikB.setM1Speed(BSpeed);
}

void setup() {
  Serial.begin(9600);
  qikF.init();
  qikB.init();
}

void loop() {
    if(analogRead(photocellPin) < 200){
      if(goingL == true){
      Serial.println(analogRead(photocellPin));
      go(360);
      }
      else{
      Serial.println(analogRead(photocellPin));
      go(180);
      }
    }
    else
    {
      qikF.setM1Brake(80);
      qikF.setM0Brake(80);
      qikB.setM1Brake(80);
      delay(100);
      goingL = !goingL;
    }
}

#include <PololuQik.h>
#include <Bounce2.h>

//tx,rx,reset
PololuQik2s12v10 qikF(3, 4, 5);
PololuQik2s12v10 qikB(10, 11, 12);

Bounce debouncer = Bounce(); 
const int buttonPin = 6;
bool runBot = false;

int targ;
int FLSpeed;
int FRSpeed;
int BSpeed;
bool goingL, goingR, goingB, goingS;
int photocellPin = 3;
int photocellReading;
 
void go(float degree)
{
  Serial.println(degree);
  static int speed = 100;
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

  pinMode(buttonPin, INPUT_PULLUP);

  debouncer.attach(buttonPin);
  debouncer.interval(1000);
}

void loop() {
      debouncer.update();

      int buttonState = debouncer.read();
      
      if (buttonState == LOW) {
          runBot = !runBot;
          
        if (runBot == false) {
          qikF.setM1Speed(0);
          qikF.setM0Speed(0);
          qikB.setM1Speed(0);
          return;
        }
       go(180);
    }     
}

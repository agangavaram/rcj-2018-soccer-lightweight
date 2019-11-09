#include <PololuQik.h>
#include <Bounce2.h>
#include <HTInfraredSeeker.h>



Bounce debouncer = Bounce();
int val;
int curr;
float error;
float errord;
float kp = .2;
int targetvalue;
volatile bool bErr = false;
const int buttonPin = 6;
const int pingPin = 7;
bool runBot = false;
bool targSet = false;
int FMotor, BMotor, RMotor, LMotor;

//////////////////////////////////////////////////////////////////////////////////////////

PololuQik2s12v10 qikB(3, 4, 5);
PololuQik2s12v10 qikF(10, 11, 12);

//////////////////////////////////////////////////////////////////////////////////////////
//Qik  M0 == South left
//Qik  M1 == North right
//Qik2 M0 == Right forward
//Qik2 M1 == Left back
void setup()
{
  qikF.init();
  qikB.init();
  Serial.begin(57600);

  InfraredSeeker::Initialize();

  pinMode(buttonPin, INPUT_PULLUP);

  debouncer.attach(buttonPin);
  debouncer.interval(1000);
  attachInterrupt(0, errfunc, HIGH);
}

void setTarg() {
  if (Serial.available())
  {
    targetvalue = Serial.parseFloat();
  }
}

void errfunc()
{
  bErr = true;
}

void go(int dir, int speed)
{
  if (Serial.available())
  {
    curr = Serial.parseFloat();
  }

  error = targetvalue - curr;
  if (error < -180) error = error + 360;
  if (error > 180) error = -(360 - error);
  Serial.println(error);

  if (error < 2 || error > -2)
  {
    error = 0;
  }
  else
  {
    error = error;
  }

  float rad = dir / (180 / PI);

  RMotor = -sin(rad) * speed;
  LMotor = sin(rad) * speed;
  FMotor = cos(rad) * speed - error;
  BMotor = -cos(rad) * speed - error;

  qikF.setM0Speed(RMotor);
  qikF.setM1Speed(LMotor);
  qikB.setM0Speed(BMotor);
  qikB.setM1Speed(FMotor);

  Serial.print("RMotor = ");
  Serial.println(RMotor);
  Serial.print("LMotor = ");
  Serial.println(LMotor);
  Serial.print("BMotor = ");
  Serial.println(BMotor);
  Serial.print("FMotor = ");
  Serial.println(FMotor);
  /*
    if (error < -2)
    {
      qikF.setM0Speed(-error * 10);
      qikF.setM1Speed(error * 10);
    }
    else if (error > 2)
    {
      qikF.setM0Speed(erro * 10);
      qikF.setM1Speed(-error * 10);
    }
    else
    {
      qikF.setM1Brake(75);
      qikF.setM0Brake(75);
    }
  */
}


void patrol()
{

  InfraredResult InfraredBall = InfraredSeeker::ReadAC();
  if (InfraredBall.Strength > 140)
  {
    go(270, 60);
    delay(1000);
    go(90, 60);
    delay(1000);
    qikF.setM1Brake(75);
    qikF.setM0Brake(75);
    qikB.setM1Brake(75);
    qikB.setM0Brake(75);
  }
  else {
    switch (InfraredBall.Direction) {
      case 3:
        go(360, 60);
        Serial.println("found in 3");
        break;

      case 4:
        go(360, 60);
        Serial.println("found in 4");
        break;

      case 5:
        qikF.setM1Brake(75);
        qikF.setM0Brake(75);
        qikB.setM1Brake(75);
        qikB.setM0Brake(75);
        Serial.println("found in 5");
        break;

      case 6:
        go(180, 60);
        Serial.println("found in 6");
        break;

      case 7:
        go(180, 60);
        Serial.println("found in 7");
        break;

      default:
        qikF.setM1Brake(75);
        qikF.setM0Brake(75);
        qikB.setM1Brake(75);
        qikB.setM0Brake(75);
        Serial.println("in default");
    }
  }
}


int getPing() {
  long duration, cm;

  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);
  cm = duration / 29 / 2;
  return cm;
}

void stayIn() {
  if (getPing() > 60) {
    Serial.println("Stay In:");
    qikF.setM0Speed(-60 - error * kp);
    qikF.setM1Speed(60 + error * kp);
  }
  else if (getPing() < 20) {
    Serial.println("Stay In:");
    qikF.setM1Speed(-60 - error * kp);
    qikF.setM0Speed(60 + error * kp);
  }
}


void loop() {
  if (bErr)
  {
    qikF.init();
    qikB.init();
    bErr = false;
  }

  debouncer.update();

  int buttonState = debouncer.read();

  if (buttonState == LOW)
  {
    runBot = true;
    buttonState == HIGH;
  }

  if (runBot == false)
  {
    qikF.setM1Speed(0);
    qikF.setM0Speed(0);
    qikB.setM1Speed(0);
    qikB.setM0Speed(0);
    runBot == true;
    return;
  }

  if (targSet == false)
  {
    Serial.println("Starting");
    setTarg();
    Serial.println(targetvalue);
    Serial.println("Target Set");
    targSet = true;
  }
  else
  {
    Serial.print("Patrol: ");
    if(getPing() > 60){
    Serial.println("Too Far");
    qikF.setM1Speed(-100 - error * kp);
    qikF.setM0Speed(100 + error * kp);
    patrol();
    }
    else if(getPing() < 20)
    {
      Serial.println("Stopping");
      qikF.setM0Speed(0);
      qikF.setM1Speed(0);
      patrol();
    }
  }
}

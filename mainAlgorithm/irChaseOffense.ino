#include <Bounce2.h>
#include <PololuQik.h>
#include <HTInfraredSeeker.h>

//tx,rx,reset
PololuQik2s12v10 qikF(3, 4, 5);
PololuQik2s12v10 qikB(10, 11, 12);

Bounce debouncer = Bounce();

volatile bool bErr = false;
int targ;
int FLSpeed;
int FRSpeed;
int BSpeed;
bool goingL, goingR, goingB, goingF, onWhite;
const int photocellPin = 3;
const int buttonPin = 6;
bool runBot = false;
int lastButtonState;
bool targSet = false;
int imuHeading() {
  int val;
  if (Serial.available()) {
    val = Serial.parseInt(); //see whatever came in on Serial.read()
    return val;
  }
}

void errfunc()
{
  bErr = true;
}

//sets target
void setTarg() {
  targ = imuHeading();
  Serial.print("Setup (heading, targ): ");
  Serial.print(imuHeading());
  Serial.print(" ");
  Serial.println(targ);
  targSet = true;
}

//PID in certain direction
void go(float degree, int speed)
{
  Serial.println(degree);
  static float pi = 3.1415;
  float rad = degree / (180 / pi);
  float err = 0;
  static float kp = 0.38;
  float adjP;
  int curr;

  //prevErr = err;
  curr = imuHeading();
  //if(curr > 180)
    //curr = curr - 180;

  if ((targ - curr) > 180)
    err = ((targ - curr) - 360) * -1;
  else if((targ - curr) < -180)
    err = ((targ - curr) + 360);
  else
     err = targ - curr;
//  
//  err = targ - curr;
  adjP = -1* (err * kp);
//  adjP = 1* (err * kp);


//ratios for kiwi bot
  FLSpeed = (sin(rad) * speed) * (sqrt(3) / 2) - ((cos(rad) * speed) / 2) + adjP; // front-left
  FRSpeed  = (-(sin(rad) * speed) * (sqrt(3) / 2)) - (cos(rad) * speed) / 2 + adjP; // front-right
  BSpeed = (cos(rad) * speed) + adjP;

  Serial.print("Speeds (L,R,B): ");
  Serial.print(FLSpeed);
  Serial.print(" ");
  Serial.println(FRSpeed);
  Serial.print(" ");
  Serial.println(BSpeed);
  Serial.println("-------------------------------");
  Serial.print("Target: ");
  Serial.println(targ);

  Serial.print("Current heading: ");
  Serial.println(curr);

  Serial.print("Current Error: ");
  Serial.println(err);
  
    qikF.setM1Speed(FLSpeed);
    qikF.setM0Speed(FRSpeed);
    qikB.setM1Speed(BSpeed);
  
  goingB, goingR, goingF, goingL = false;
}

//IR detects ball in zone and heads towards it
void find_ball()
{
  InfraredResult InfraredBall = InfraredSeeker::ReadAC();

//if the ball is close to the robot
  if (InfraredBall.Strength > 150)
  {
    goingF = true;
    goingL, goingR, goingB = false;
    go(90, 100);
  }
//if the ball is behind it
  else if (InfraredBall.Strength < 30)
  {
    goingB = true;
    goingL, goingR, goingF = false;
    go(270, 60);
  }
  else {
    switch (InfraredBall.Direction) {
      //ball in 3
      case 3:
        goingL = true;
        goingR, goingB, goingF = false;
        go(30, 60);
        break;

      //ball in 4
      case 4:
        goingL = true;
        goingR, goingB, goingF = false;
        go(60, 65);
        Serial.println("found in 4");
        break;

      //ball in 5
      case 5:
        goingF = true;
        goingR, goingB, goingL = false;
        go(90, 70);
        Serial.println("found in 5");
        break;

      //ball in 6
      case 6:
        goingR = true;
        goingL, goingB, goingF = false;
        go(120, 65);
        Serial.println("found in 6");
        break;

      //ball in 7
      case 7:
        goingR = true;
        goingL, goingB, goingF = false;
        go(150, 60);
        Serial.println("found in 7");
        break;

      //go backwards if doesn't see
      default:
        goingB = true;
        goingL, goingR, goingF = false;
        go(270, 60);
        Serial.println("going back");
    }
  }
}

//brakes the motors at max speed
void stayIn() {
  onWhite = true;
  qikF.setM1Brake(127);
  qikF.setM0Brake(127);
  qikB.setM1Brake(127);
  delay(2000);
}

void setup() {
  Serial.begin(57600);
  qikF.init();
  qikB.init();
  InfraredSeeker::Initialize();
  pinMode(buttonPin, INPUT_PULLUP);

  debouncer.attach(buttonPin);
  debouncer.interval(1000);

  attachInterrupt(0, errfunc, HIGH);
}

void loop() {

  //checks the errors of the motors, resets motors if bErr == true
  if(bErr)
  {
    qikF.init();
    qikB.init();
    bErr = false;
  }

  debouncer.update();

  //reads the state of the button
  int buttonState = debouncer.read();

  //if button LOW sets runBot boolean to true which allows the bot to run
  if (buttonState == LOW) {
    runBot = true;
    buttonState == HIGH;
  }

  if (runBot == false) {
    Serial.println(imuHeading());
    qikF.setM1Speed(0);
    qikF.setM0Speed(0);
    qikB.setM1Speed(0);
    return;
  }

  //checks whether the target is set yet
  if (targSet == false)
    setTarg();
  else {
    if (analogRead(photocellPin) < 300){
      Serial.print("Finding Ball: ");
      Serial.println(analogRead(photocellPin));
      find_ball();
    }
    else{
      Serial.print("Staying In: ");
      Serial.println(analogRead(photocellPin));
      stayIn();
    }
  }
}

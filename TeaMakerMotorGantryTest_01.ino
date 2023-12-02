#include <Stepper.h>
#include <Servo.h>

const int stepsPerRevolution = 200;
Servo armservo;
Servo grabberservo;
int pos = 95;

// initialize the stepper library on pins 30 through 33:
Stepper myStepper(stepsPerRevolution, 30, 31, 32, 33);

void setup() {
  myStepper.setSpeed(60);
  armservo.attach(34);
  grabberservo.attach(35);
  Serial.begin(9600);
}

void loop() {
  Serial.println("clockwise");
  myStepper.step(stepsPerRevolution);
  delay(500);
  armservo.write(60);
  delay(500);
  grabberservo.write(0);
  delay(1000);
  Serial.println("counterclockwise");
  myStepper.step(-stepsPerRevolution);
  delay(500);
  armservo.write(140);
  delay(500);
  grabberservo.write(180);
  delay(1000);
}


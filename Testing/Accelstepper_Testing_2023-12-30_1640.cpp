#include <AccelStepper.h>
#include <Servo.h>

// Define Stepper and Servo objects
AccelStepper elevatorRack(AccelStepper::FULL4WIRE, 6, 7, 8, 9);
Servo pivotServo;

// Constants
const int rackPin1 = 6;
const int rackPin2 = 7;
const int rackPin3 = 8;
const int rackPin4 = 9;
const int pivotServoPin = 4;

void setup() {
  Serial.begin(9600);

  // Attach rack/elevator pins
  elevatorRack.setMaxSpeed(3000);
  elevatorRack.setAcceleration(1000);

  // Attach pivotServo pin
  pivotServo.attach(pivotServoPin);
}

void loop() {

  // Move the rack up (2 cm)
  Serial.println("moving up");
  elevatorRack.moveTo(400);
  while (elevatorRack.run()) {
    // Run the stepper
  }
  delay(1000); // Wait for 1 second

  // Move the rack down
  Serial.println("moving down");
  elevatorRack.moveTo(0);
  while (elevatorRack.run()) {
    // Run the stepper
  }
  
  delay(1000); // Wait for 1 second

  pivotServo.write(90);
  delay(2000);
  pivotServo.write(100);
  delay(2000);
}

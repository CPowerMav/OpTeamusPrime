#include <Servo.h>
#include <Stepper.h>
#include "TeaTypes.h"

// Define pin numbers
const int pivotServoPin = 2;
const int grabberServoPin = 3;
const int loadButtonPin = 4;
const int nextButtonPin = 5;
const int rotaryInputPin = 6;
const int elevatorRackStepPin = 7;
const int elevatorRackDirPin = 8;
const int cupPresencePin = 9;
const int relayPin = 10;
const int temperatureSensorPin = A0;
const int ultrasonicTrigPin = 11;
const int ultrasonicEchoPin = 12;

// Define constants
const int STEPS_PER_REVOLUTION = 200;
const int dispenseTime = 5000;  // Adjust as needed

// Create Servo and Stepper objects
Servo pivotServo;
Servo grabberServo;
Stepper elevatorRack(STEPS_PER_REVOLUTION, elevatorRackStepPin, elevatorRackDirPin);

// Other variables
TeaType currentTea = {"White Tea", 270000, 79};  // Default tea type
int teaTimeAdjustment = 0;

void setup() {
  // Initialization code here
}

void loop() {
  startupInit();
  loadGrabber();
  teaSelection();
  progAdjust();
  preFlight();
  heatWater();
  steepFunction();
  disposeBag();
  shutDown();
}

void startupInit() {
  // Code for startup initialization
  pivotServo.write(90);
  // Move gantry until limit switch is triggered
  while (digitalRead(elevatorRackLimitSwitchPin) == HIGH) {
    elevatorRack.step(1);
  }
  elevatorRack.step(-10);  // Adjust as needed
  pivotServo.write(0);
}

void loadGrabber() {
  // Code for loading the grabber
  grabberServo.write(90);  // Close grabber
  while (digitalRead(loadButtonPin) == LOW) {
    // Wait for load button press
  }
  grabberServo.write(0);  // Open grabber
  while (digitalRead(loadButtonPin) == HIGH) {
    // Wait for load button release
  }
  // Wait for next button press
  while (digitalRead(nextButtonPin) == LOW) {
    // Wait for next button press
  }
}

void teaSelection() {
  // Code for tea selection using rotary encoder
  // Display tea options and adjust currentTea accordingly
  // Wait for next button press
}

void progAdjust() {
  // Code for adjusting tea time using rotary encoder
  // Display current teaTime and adjust teaTimeAdjustment accordingly
  // For example, increase tea time by 30 seconds for each rotary step
  currentTea.teaTime += teaTimeAdjustment * 30000;
  // Wait for next button press
}

void preFlight() {
  // Code for pre-flight checks
  // Check ultrasonic sensor and boolean sensor
  // Wait for next button press
}

void heatWater() {
  // Code for heating water
  // Pump water, energize heating coil, measure temperature, dispense water
  // Wait for next button press
}

void steepFunction() {
  // Code for steeping the tea bag
  // Lower tea bag, countdown, raise tea bag
  // Move on to next function automatically
}

void disposeBag() {
  // Code for disposing of the tea bag
  // Listen to ultrasonic sensor, move gantry, rotate arm, release grabber, move gantry
  // Wait for next button press
}

void shutDown() {
  // Code for shutting down the unit
  // Activate latching circuit
}

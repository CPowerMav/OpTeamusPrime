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
const int heatingCoil = ?;
const int cupPresencePin = 9;
const int relayPin = 10;
const int temperatureSensorPin = A0;
const int ultrasonicTrigPin = 11;
const int ultrasonicEchoPin = 12;
const int waterReservoir = ?;
const int waterPump = ?;
const int airPump = ?;


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

/**
// Define Tea Types - External Include?
const struct teaTypes {
  const char* name;
  unsigned long time;
  int temp;
  
} teaType[] = {
  {"White Tea", 270000, 79},
  {"Green Tea", 240000, 79},
  {"Black Tea", 210000, 91},
  {"Oolong Tea", 210000, 91},
  {"Herbal Tea", 800000, 99},
};
**/


// Define constants
const int STEPS_PER_REVOLUTION = 200;
const int dispenseTime = 5000;  // Adjust as needed

// Create Servo and Stepper objects
Servo pivotServo;
Servo grabberServo;
Stepper elevatorRack(STEPS_PER_REVOLUTION, elevatorRackStepPin, elevatorRackDirPin);


// Define variables
int currentTeaIndex = 0;
unsigned long currentTeaTime = teaTypes[currentTeaIndex].time;
int currentTeaTemp = teaTypes[currentTeaIndex].temp;
TeaType currentTea = {"White Tea", 270000, 79};  // Default tea type
int teaTimeAdjustment = 0;

//Setup - Runs once

void setup() {
  // Initialization code here
}

//Main program

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

// Functions called by main program

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
  // Rotary encoder allows the user to scroll through a list of teas from an array called "teaTypes"
  int encoderValue = 0;
  int encoderLastValue = 0;
  int encoderMaxValue = sizeof(teaTypes) / sizeof(teaTypes[0]) - 1;
  while (digitalRead(nextButton) == HIGH) {
    encoderValue += (digitalRead(rotaryInput) == HIGH) - (digitalRead(rotaryInput) == LOW);
    if (encoderValue < 0) {
      encoderValue = encoderMaxValue;
    } else if (encoderValue > encoderMaxValue) {
      encoderValue = 0;
    }
    if (encoderValue != encoderLastValue) {
      currentTeaIndex = encoderValue;
      currentTeaTime = teaTypes[currentTeaIndex].time;
      currentTeaTemp = teaTypes[currentTeaIndex].temp;
      encoderLastValue = encoderValue;
    }
    delay(50);
  }
}

void progAdjust() {
  // Allows the user to tweak or adjust teaTime variable by using the rotary encoder (rotaryInput) to add or subtract time from teaTime variable in increments of 30 seconds (30000ms)
  int encoderValue = 0;
  int encoderLastValue = 0;
  while (digitalRead(nextButton) == HIGH) {
    encoderValue += (digitalRead(rotaryInput) == HIGH) - (digitalRead(rotaryInput) == LOW);
    if (encoderValue != encoderLastValue) {
      currentTeaTime += encoderValue * 30000;
      encoderLastValue = encoderValue;
    }
    delay(50);
  }
}

void preFlight() {
  // Check ultrasonic sensor to ensure a cup is placed for dispensing hot water into it
  if (digitalRead(cupPresence) == LOW) {
    // Cup is present
  } else {
    // Cup is not present
  }
  
  // Check boolean sensor to ensure the water reservoir has enough water to perform water heating/brewing/steeping
  if (digitalRead(waterReservoir) == HIGH) {
    // Water reservoir has enough water
  } else {
    // Water reservoir is empty
  }
  
  // Wait for next button to be pressed
  while (digitalRead(nextButton) == HIGH) {
    delay(100);
  }
}

void heatWater() {
  // Water is pumped from the reservoir to the boiler
  // Heating coil is energized by activating a pin connected to a relay
  digitalWrite(heatingCoil, HIGH);
  // measure temperature, dispense water
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
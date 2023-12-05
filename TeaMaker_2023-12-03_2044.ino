#include <Servo.h>
#include <Stepper.h>

// Define Analog Sensor Pins

const int rotaryInput = 4;
const int cupPresence = 5;
const int waterReservoir = 6;
const int heatingCoil = 7;
const int temperatureSensor = 8;

// Define Digital Input Pins


// Define user interraction pins
const int loadButton = 2;
const int nextButton = 3;


// Define Analog Input Pins
const int waterReservoir = 6; // Water presence sensor

// Define Motor Pins
const int pivotServo = 9;
const int elevatorRack = 10;
const int grabberServo = 11;

// Define tea types
const struct TeaType {
  const char* name;
  unsigned long time;
  int temp;
} teaTypes[] = {
  {"White Tea", 270000, 79},
  {"Green Tea", 240000, 79},
  {"Black Tea", 210000, 91},
  {"Oolong Tea", 210000, 91},
  {"Herbal Tea", 800000, 99},
};

// Define variables
int currentTeaIndex = 0;
unsigned long currentTeaTime = teaTypes[currentTeaIndex].time;
int currentTeaTemp = teaTypes[currentTeaIndex].temp;

// Define functions
void startupInit() {
  // Rotate arm to 90 degrees
  rotateArm(90);
  
  // Move gantry until limit switch is triggered, then stop
  while (digitalRead(elevatorRack) == LOW) {
    moveGantry(1);
  }
  moveGantry(0);
  
  // Rotate arm to 0 degrees
  rotateArm(0);
}

void loadGrabber() {
  // Close grabber servo
  closeGrabber();
  
  // Listen for button press
  while (digitalRead(loadButton) == LOW) {
    // Open grabberServo while button is pressed
    openGrabber();
  }
  
  // When the button is released, the grabberServo returns to closed
  closeGrabber();
  
  // Wait for next button to be pressed
  while (digitalRead(nextButton) == HIGH) {
    delay(100);
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
  
  // Temperature sensor measures the water temperature during heating until it reaches the
// External includes
#include <Servo.h>
#include <Stepper.h>
#include "TeaTypes.h"

// Define Digital IO pin numbers
const int pivotServoPin = 22;
const int grabberServoPin = 23;
const int loadButtonPin = 24;
const int nextButtonPin = 25;
const int rotaryInputPin = 26;
const int elevatorRackStepPin = 27;
const int elevatorRackDirPin = 28;
const int elevatorRackLimitSwitchPin = 29;
const int heatingCoil = 30;

const int cupPresencePin = 31;
const int ultrasonicTrigPin = 32;
const int ultrasonicEchoPin = 33;

const int waterPump = 34;
const int airPump = 35;

// Define Analog IO Pins
const int temperatureSensorPin = A0; 
const int waterReservoir = A1;
const int waterFill = A2;  // Water level probe inside boiler for regular size
const int waterFillMax = A3; // Water level probe inside boiler for max size

// Define constants
const int STEPS_PER_REVOLUTION = 200; // Stepper value
const int dispenseTime = 5000;  // Adjust as needed
const float Rref = 50000.0;  // Reference resistance
const float nominal_temeprature = 25.0;  // Nominal temperature in Celsius
const float nominal_resistance = 50000.0;  // Nominal resistance at nominal temperature
const float beta = 3950.0;  // Beta value of the NTC thermistor

// Create Servo and Stepper objects
Servo pivotServo;
Servo grabberServo;
Stepper elevatorRack(STEPS_PER_REVOLUTION, elevatorRackStepPin, elevatorRackDirPin);


// Define Tea Types - External Include?
const struct teaTypes {
  const char* name;
  unsigned long time;
  int temp;
}

teaType[] = {
  {"White Tea", 270000, 79},
  {"Green Tea", 240000, 79},
  {"Black Tea", 210000, 91},
  {"Oolong Tea", 210000, 91},
  {"Herbal Tea", 800000, 99},
};


// Define variables
int currentTeaIndex = 0;
unsigned long currentTeaTime = teaTypes[currentTeaIndex].time;
int selectedTeaTemp = teaTypes[currentTeaIndex].temp;
TeaType currentTea = {"White Tea", 270000, 79};  // Default tea type
int teaTimeAdjustment = 0;

//Setup - Runs once

void setup() {
	Serial.begin(9600);
	Serial.print("The program has begun");
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

void startupInit() { // Code for startup initialization
  pivotServo.write(90); // Rotate grabber to EAST
  delay(1000); // Wait a second for it to arrive
  
  // Move gantry until limit switch is triggered
  while (digitalRead(elevatorRackLimitSwitchPin) == HIGH) {
    elevatorRack.step(1);
  }
  elevatorRack.step(-10);  // Adjust as needed
  pivotServo.write(0); // Rotate grabber to SOUTH
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
      selectedTeaTemp = teaTypes[currentTeaIndex].temp;
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
  
  digitalWrite(heatingCoil, HIGH);   // Heating coil is energized by activating a pin connected to a relay

  // Wait for the heating coil to warm up (adjust as needed)
  delay(2000);

  while (true) {
    int temperatureReading = analogRead(temperatureSensorPin);
    float temperatureCelsius = calculateTemperature(temperatureReading);

    Serial.print("Temperature: ");
    Serial.println(temperatureCelsius);

    // Check if the current temperature is below the target temperature
    if (temperatureCelsius < currentTeaTemp) {
      // Continue heating
      // You may need to adjust the heating process based on your system characteristics
    } else {
      // Stop heating
      digitalWrite(heatingCoil, LOW);
      break;
    }
  }

  // Add additional code here for dispensing water into the cup based on dispenseTime
  // This can include controlling pumps or valves
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

float calculateTemperature(int temperatureSensorPin) {
  float R = Rref * (1023.0 / (float)temperatureSensorPin - 1.0); // Calculate NTC resistance
  float T = 1.0 / (1.0 / (nominal_temeprature + 273.15) + log(R / nominal_resistance) / beta);
  T -= 273.15; // convert absolute temp to C
  return T;
}
// Main code
#include "TeaDatabase.h"
#include "BrewFunctions.h"
#include <Encoder.h>

// Rotary encoder and button pins
const int encoderPinA = 5;
const int encoderPinB = 6;
const int buttonPin = 7;

Encoder teaSelector(encoderPinA, encoderPinB);

// Define variables to store the current program
int currentProgram = 0;

void setup() {
    pinMode(buttonPin, INPUT_PULLUP);
    Serial.begin(9600);
    // ... (initialize other components if needed)
}

void brewSelectedTea(const TeaType& selectedTea) {
    // Execute the brew function with the selected temperature
    brewTea(selectedTea.brewTemp);
    Serial.println("Brewing " + String(selectedTea.name) + " at temperature: " + String(selectedTea.brewTemp) + "°C for " + String(selectedTea.brewTime) + " seconds");
    // Add your specific brewing actions here
}

void loop() {
    // Read the rotary encoder
    int encoderValue = teaSelector.read();

    // Map the encoder value to a program number
    currentProgram = map(encoderValue, 0, 1023, 0, sizeof(teaTypes) / sizeof(teaTypes[0]) - 1);
    currentProgram = constrain(currentProgram, 0, sizeof(teaTypes) / sizeof(teaTypes[0]) - 1);

    // Check if the button is pressed
    if (digitalRead(buttonPin) == LOW) {
        // Button is pressed, so start the tea making process
        Serial.println("Button pressed");

        // Determine the selected tea type based on the current program
        const TeaType& selectedTea = teaTypes[currentProgram];

        // Brew the selected tea
        brewSelectedTea(selectedTea);
        // Add any additional actions after brewing if needed
    }

    // ... (rest of your loop logic)
}

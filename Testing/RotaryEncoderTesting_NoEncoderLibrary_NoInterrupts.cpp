#include <Arduino.h>
#include <Encoder.h>

// Define Rotary Encoder pins
const int rotaryA = 23; // DT Pin - Interrupt capable pin
const int rotaryB = 24; // CLK Pin - Interrupt capable pin

Encoder selectorKnob(rotaryA, rotaryB);

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Read changes from the rotary encoder
  int selectorKnobValue = selectorKnob.read();

  // Print the current value to the Serial Monitor
  Serial.print("Tea Option: ");
  Serial.println(selectorKnobValue);

  // Add a small delay to avoid rapid changes due to noise
  delay(100);
}
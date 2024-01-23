#include <Encoder.h>

// Define interrupt pins for the rotary encoder
const int rotaryA = 23;
const int rotaryB = 24;

Encoder selectorKnob(rotaryA, rotaryB);
volatile long encoderValue = 0;

void handleEncoder() {
  encoderValue = selectorKnob.read();
}

void setup() {
  Serial.begin(9600);

  pinMode(rotaryA, INPUT_PULLUP);
  pinMode(rotaryB, INPUT_PULLUP);

  // Enable the interrupts for the rotary encoder
  attachInterrupt(digitalPinToInterrupt(rotaryA), handleEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(rotaryB), handleEncoder, CHANGE);
}

void loop() {
  // Your main code here
  Serial.print("Encoder Value: ");
  Serial.println(encoderValue);

  delay(100); // Adjust the delay as needed
}
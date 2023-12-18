// These constants won't change. They're used to give names to the pins used:
const int analogInPin = A0; // Analog input pin that the potentiometer is attached to
const int ledPin = 13; // pin that the LED is attached to
const int waterLevelThreshold = 200;
const int sensorMax = 550; // Using 50K Ohm resistor, this is the max value read when the water tank was full
const int sensorMin = 5; // Using 50K Ohm resistor, this is the min value read when the water tank was empty

// Variables
int sensorValue = 0;  // value read from the water level sensor


void setup() {
  Serial.begin(9600); // open a serial connection
  pinMode(ledPin, OUTPUT); // set the LED pin as an output
  digitalWrite(ledPin, LOW); // Start with the LED off
}

void loop() {
  // store the value reported by the sensor in a variable
  // read the analog in value:
  sensorValue = analogRead(analogInPin);

  // if the value is greater than the threshold, turn OFF "Add Water Light"
  if (sensorValue > waterLevelThreshold) {
    digitalWrite(ledPin, LOW); // turn on the LED
    Serial.print("Water is PRESENT becuase the sensor is reading: ");
    Serial.print(sensorValue);
    Serial.print(" which is higher than the threshold: ");
    Serial.println(waterLevelThreshold);
  }
  // if it's less than the threshold, turn ON "Add Water Light"
  else {
    digitalWrite(ledPin, HIGH);
    Serial.print("Water is EMPTY becuase the sensor is reading: ");
    Serial.print(sensorValue);
    Serial.print(" which is lower than the threshold: ");
    Serial.println(waterLevelThreshold);
  }
  delay(100);
}

// Include the file with the readThermistor function
#include "readThermistor.ino"

// Define the pin where the thermistor is connected
#define THERMISTOR_PIN A0

// Define the fixed resistor value (in ohms)
#define RESISTOR_VALUE 10000

// Define the thermistor specifications
#define THERMISTOR_R25 50000 // Resistance at 25°C (in ohms)
#define THERMISTOR_B 3950 // B-value (in kelvins)

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
}

void loop() {
  // Call the readThermistor function and pass the parameters
  float temperature = readThermistor(THERMISTOR_PIN, RESISTOR_VALUE, THERMISTOR_R25, THERMISTOR_B);

  // Print the temperature to the serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  // Wait for a second
  delay(1000);
}
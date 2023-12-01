// Temperature calculations
float calculateTemperature(int analogInput) {
  float R = Rref * (1023.0 / (float)analogInput - 1.0); // Calculate NTC resistance
  float T = 1.0 / (1.0 / (nominal_temperature + 273.15) + log(R / nominal_resistance) / beta);
  T -= 273.15; // convert absolute temp to C
  return T;
}

// Define the pins used for the various components
const int buttonPin = 2;      // Pin used for the button
const int programPin = A0;    // Pin used for the program selector potentiometer
const int heaterPin = 3;      // Pin used to activate the water heater
const int pumpPin = 4;        // Pin used to activate the water pump
const int tempPin = A1;       // Pin used to read the temperature sensor

// Define the various temperature settings for each tea type
const int greenTeaTemp = 75;  // Temperature for green tea
const int blackTeaTemp = 85;  // Temperature for black tea
const int herbalTeaTemp = 90; // Temperature for herbal tea

// Define variables to store the current program and temperature
int currentProgram = 0;
int currentTemp = 0;

void setup() {
  // Initialize the inputs and outputs
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(programPin, INPUT);
  pinMode(heaterPin, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  Serial.begin(9600); // For debugging purposes
}

void loop() {
  // Read the program selector potentiometer
  int programValue = analogRead(programPin);

  // Map the program value to a program number (0, 1, or 2)
  currentProgram = map(programValue, 0, 1023, 0, 2);

  // Check if the button is pressed
  if (digitalRead(buttonPin) == LOW) {
    // Button is pressed, so start the tea making process
    Serial.println("Button pressed");

    // Determine the target temperature based on the current program
    switch (currentProgram) {
      case 0:
        currentTemp = greenTeaTemp;
        break;
      case 1:
        currentTemp = blackTeaTemp;
        break;
      case 2:
        currentTemp = herbalTeaTemp;
        break;
      default:
        // This should never happen, but just in case...
        currentTemp = 0;
        break;
    }

    // Activate the water heater
    digitalWrite(heaterPin, HIGH);

    // Wait until the target temperature is reached
    while (calculateTemperature(analogRead(tempPin)) < currentTemp) {
      // Do nothing, just wait
      delay(100);
    }

    // Deactivate the water heater
    digitalWrite(heaterPin, LOW);

    // Activate the water pump
    digitalWrite(pumpPin, HIGH);

    // Wait a few seconds for the water to flow into the cup
    delay(5000);

    // Deactivate the water pump
    digitalWrite(pumpPin, LOW);

    // Tea making process is complete
    Serial.println("Tea making process complete");
  }
}

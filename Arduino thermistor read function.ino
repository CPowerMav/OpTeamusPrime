// This function reads and converts the analog value from a thermistor to the temperature value in degrees Celsius
// It uses the Steinhart-Hart equation to calculate the temperature from the resistance
// It takes four parameters: the pin where the thermistor is connected, the fixed resistor value, the thermistor R25 value, and the thermistor B-value
// It returns the temperature in degrees Celsius as a float

float readThermistor(int pin, float resistor, float r25, float b) {
  // Define the Steinhart-Hart coefficients
  #define SH_A 0.001129148 // A coefficient
  #define SH_B 0.000234125 // B coefficient
  #define SH_C 0.0000000876741 // C coefficient

  // Define the ADC resolution and reference voltage
  #define ADC_RESOLUTION 1023 // 10-bit ADC
  #define REF_VOLTAGE 5.0 // 5V reference voltage

  // Read the analog value from the thermistor pin
  int analogValue = analogRead(pin);

  // Convert the analog value to voltage
  float voltage = analogValue * REF_VOLTAGE / ADC_RESOLUTION;

  // Calculate the thermistor resistance from the voltage divider formula
  float thermistorResistance = resistor * voltage / (REF_VOLTAGE - voltage);

  // Calculate the temperature from the Steinhart-Hart equation
  float temperature = 1 / (SH_A + SH_B * log(thermistorResistance / r25) + SH_C * pow(log(thermistorResistance / r25), 3));

  // Convert the temperature from kelvins to degrees Celsius
  temperature = temperature - 273.15;

  // Return the temperature
  return temperature;
}
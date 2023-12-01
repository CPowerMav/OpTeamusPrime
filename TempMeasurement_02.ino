// Temperature Measurement

#define ntc_pin A0         // Pin,to which the voltage divider is connected
#define sensor_rail 2        // 5V for the voltage divider
#define nominal_resistance 50000       //Nominal resistance at 25⁰C
#define nominal_temeprature 25   // temperature for nominal resistance (almost always 25⁰ C)
#define samplingrate 5    // Number of samples 
#define beta 3950  // The beta coefficient or the B value of the thermistor (usually 3000-4000) check the datasheet for the accurate value.
#define Rref 10000   //Value of  resistor used for the voltage divider
#define mosfetpin 3        // 5V for switching 12v circuit
int samples = 0;   //array to store the samples
unsigned long previousMillis = 0; 
const long interval = 3000;  // interval at which to blink (milliseconds)

// FUNCTION VOID SETUP MASTER
void setup(void) {
  pinMode(sensor_rail, OUTPUT);
  pinMode(mosfetpin, OUTPUT);
  Serial.begin(9600);   //initialize serial communication at a baud rate of 9600
}

// FUNCTION Temperature Monitor Loop Program
void loop(void) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    uint8_t i;
    float average;
    samples = 0;
    // take voltage readings from the voltage divider
    digitalWrite(sensor_rail, HIGH);
    digitalWrite(mosfetpin, HIGH);
    for (i = 0; i < samplingrate; i++) {
      samples += analogRead(ntc_pin);
      delay(10);
    }
    digitalWrite(sensor_rail, LOW);
    digitalWrite(mosfetpin, LOW);
    average = 0;
    average = samples / samplingrate;

    // Calculate NTC resistance
    average = 1023 / average - 1;
    average = Rref / average;

    float temperature;
    temperature = average / nominal_resistance;            // (R/Ro)
    temperature = log(temperature);                        // ln(R/Ro)
    temperature /= beta;                                   // 1/B * ln(R/Ro)
    temperature += 1.0 / (nominal_temeprature + 273.15);   // + (1/To)
    temperature = 1.0 / temperature;                       // Invert
    temperature -= 273.15;                                 // convert absolute temp to C

    Serial.print("Temperature ");
    Serial.print(temperature);

    delay(interval);
    }
}



float calculateTemperature(int analogInput) {
  float R = Rref * (1023.0 / (float)analogInput - 1.0); // Calculate NTC resistance
  float T = 1.0 / (1.0 / (nominal_temeprature + 273.15) + log(R / nominal_resistance) / beta);
  T -= 273.15; // convert absolute temp to C
  return T;
}
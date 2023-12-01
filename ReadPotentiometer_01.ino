
int const potPin = A0; // analog pin used to connect the potentiometer
int potVal;  // variable to read the value from the analog pin
int chrisVal;  // variable to carry remapped value from pot

void setup() {
  Serial.begin(9600); // open a serial connection to your computer
}

void loop() {
  potVal = analogRead(potPin); // read the value of the potentiometer
  chrisVal = map(potVal, 0, 1023, 0,100); // remap pot value to percentage
  // print out the value to the serial monitor
  Serial.print("\n Potentiometer Value: ");
  Serial.print(chrisVal);

  // wait for the servo to get there
  delay(15);
}



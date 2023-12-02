// C++ code
//
/*
  Main sequence here.
*/

#include <Servo.h>

int cupLed = 0;

int cupDistance = 0;

int distanceSet = 0;

int north = 0;

int east = 0;

int brewSelector = 0;

int goButton = 0;

int teaTimeWhite = 0;

int teaTimeOolong = 0;

int teaTimeGreen = 0;

int teaTimeHerbal = 0;

int teaTimeBlack = 0;

int brewTime = 0;

int brewTemp = 0;

int teaTempWhite = 0;

int teaTempGreen = 0;

int teaTempOolong = 0;

int teaTempBlack = 0;

int teaTempHerbal = 0;

int selectedBrewMode = 0;

int Greenie = 0;

int currentTemp = 0;

int unnamed = 0;

int i = 0;

int j = 0;

Servo servo_3;

long readUltrasonicDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
}

void setup()
{
  servo_3.attach(3, 500, 2500);
  pinMode(A0, INPUT);
  pinMode(2, INPUT);
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(A1, INPUT);
  pinMode(12, OUTPUT);

  distanceSet = 50;
  north = 0;
  east = 90;
  teaTimeWhite = 270000;
  teaTimeGreen = 240000;
  teaTimeOolong = 240000;
  teaTimeBlack = 210000;
  teaTimeHerbal = 900000;
  teaTempWhite = 79;
  teaTempGreen = 79;
  teaTempOolong = 91;
  teaTempBlack = 91;
  teaTempHerbal = 100;
  servo_3.write(east);
}

void loop()
{
  // Set initial movements and variables.
  cupDistance = 0.01723 * readUltrasonicDistance(9, 8);
  brewSelector = map(analogRead(A0), 0, 1023, 0, 4);
  goButton = digitalRead(2);
  // Selection sequence begins. Once goButton is
  // presseed, move on to main brewing sequence.
  if (goButton == LOW) {
    delay(15); // Wait for 15 millisecond(s)
    // Tea type selection (brewSelector)
    if (brewSelector == 0) {
      Serial.println("Selecting: Green Tea 0");
      brewTime = teaTimeGreen;
      brewTemp = teaTempGreen;
    }
    if (brewSelector == 1) {
      Serial.println("Selecting: Black Tea 1");
      brewTime = teaTimeBlack;
      brewTemp = teaTempBlack;
    }
    if (brewSelector == 2) {
      Serial.println("Selecting: Oolong Tea 2");
      brewTime = teaTimeOolong;
      brewTemp = teaTempOolong;
    }
    if (brewSelector == 3) {
      Serial.println("Selecting: White Tea 3");
      brewTime = teaTimeWhite;
      brewTemp = teaTempWhite;
    }
    if (brewSelector == 4) {
      Serial.println("Selecting: Herbal Tea 4");
      brewTime = teaTimeHerbal;
      brewTemp = teaTempHerbal;
    }
  } else {
    if (cupDistance > distanceSet) {
      Serial.println("Please place a cup!");
      delay(3000); // Wait for 3000 millisecond(s)
    } else {
      // Main Sequence: Once goButton is pressed.
      // Main sequence here.
      // Main sequence here.
      // Main sequence here.
      Serial.println("The go button has been pressed...");
      delay(500); // Wait for 500 millisecond(s)
      Serial.println("Begin main sequence.");
      Serial.print("Program brew time is: ");
      Serial.println(brewTime);
      Serial.print("Program brew temp is: ");
      Serial.println(brewTemp);
      delay(1000); // Wait for 1000 millisecond(s)
      servo_3.write(north);
      Serial.println("Servo north. Represents moving water spout into place.");
      delay(5000); // Wait for 5000 millisecond(s)
      digitalWrite(13, HIGH);
      Serial.println("Boiler ON! Light on represents heater on.");
      while (currentTemp < brewTemp) {
        // Water heating sequence (boilerSequence)
        // Check temp every 1 second.
        delay(1000); // Wait for 1000 millisecond(s)
        currentTemp = (-40 + 0.488155 * (analogRead(A1) - 20));
        Serial.print("Waiting for temperature to reach: ");
        Serial.println(brewTemp);
        Serial.print("Current Temp is: ");
        Serial.println(currentTemp);
        Serial.print("Difference to brewTemp is: ");
        Serial.println((brewTemp - currentTemp));
      }
      // The water has reached brewTemp. Turn the heater
      // off. Execute water dispensing function here.
      // (dispenseSequence))
      digitalWrite(13, LOW);
      Serial.println("Boiler is OFF.");
      Serial.println("Moving dispensing arm into position.");
      servo_3.write(east);
      delay(3000); // Wait for 3000 millisecond(s)
      Serial.println("Displenser ready.");
      // Activate pumps to dispense hot water to cup.
      digitalWrite(12, HIGH);
      Serial.println("Pump On. Water Dispensing...");
      delay(5000); // Wait for 5000 millisecond(s)
      digitalWrite(12, LOW);
      // Dispensing complete. Activate steep function
      // (steepSequence)
      Serial.println("Pump off. Dispensing complete. Execute steeping functions. (steepSequence)");
      // Armature moves to lower tea into cup.
      servo_3.write(north);
      delay(3000); // Wait for 3000 millisecond(s)
      Serial.println("Arm is lowered into tea cup.");
      Serial.print("Tea steeping. Waiting for brewTime to complete.");
      for (i = 10; i >= 0; i -= 1) {
        Serial.println((i + i));
        delay(1000); // Wait for 1000 millisecond(s)
      }
      // Steeping function has completed. Execute exit
      // function. (closeoutSequence)
      Serial.println("Steep complete. Execute closeoutSequence.");
      Serial.print("Start shutdown sequence here.");
      delay(80000); // Wait for 80000 millisecond(s)
    }
  }
}

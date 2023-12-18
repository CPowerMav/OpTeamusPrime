// Include the Servo library
#include <Servo.h>

// Define the pin for the servo signal
#define SERVO_PIN 9

// Define the pin for the encoder signal
#define ENCODER_PIN 2

// Define the desired angle for the servo
#define TARGET_ANGLE 90

// Define the tolerance for the angle error
#define TOLERANCE 5

// Define the timeout for the servo movement in milliseconds
#define TIMEOUT 1000

// Create a Servo object
Servo servo;

// Create a variable to store the current angle from the encoder
int current_angle = 0;

// Create a variable to store the start time of the servo movement
unsigned long start_time = 0;

// Create a flag to indicate if the servo is moving or not
bool moving = false;

// Create a flag to indicate if an error has occurred or not
bool error = false;

void setup() {
  // Attach the servo to the pin
  servo.attach(SERVO_PIN);

  // Set the encoder pin as input with pull-up resistor
  pinMode(ENCODER_PIN, INPUT_PULLUP);

  // Attach an interrupt to the encoder pin to read the angle
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), read_angle, CHANGE);

  // Start the serial monitor
  Serial.begin(9600);
}

void loop() {
  // If the servo is not moving and no error has occurred, start moving it to the target angle
  if (!moving && !error) {
    // Write the target angle to the servo
    servo.write(TARGET_ANGLE);

    // Record the start time of the movement
    start_time = millis();

    // Set the moving flag to true
    moving = true;
  }

  // If the servo is moving, check if it has reached the target angle or timed out
  if (moving) {
    // Calculate the angle error as the difference between the target and current angles
    int angle_error = TARGET_ANGLE - current_angle;

    // Check if the angle error is within the tolerance range
    if (abs(angle_error) <= TOLERANCE) {
      // The servo has reached the target angle, stop moving it
      servo.detach();

      // Set the moving flag to false
      moving = false;

      // Print a success message
      Serial.println("Servo reached target angle!");
    }

    // Check if the timeout has elapsed since the start of the movement
    else if (millis() - start_time >= TIMEOUT) {
      // The servo has not reached the target angle in time, stop moving it
      servo.detach();

      // Set the moving flag to false
      moving = false;

      // Set the error flag to true
      error = true;

      // Print an error message
      Serial.println("Servo timed out!");
    }
  }
}

// This function is called by the interrupt when the encoder signal changes
void read_angle() {
  // Read the current state of the encoder pin
  int state = digitalRead(ENCODER_PIN);

  // Update the current angle based on the state change
  // This assumes that one pulse corresponds to one degree of rotation
  // You may need to adjust this depending on your encoder type and resolution
  if (state == HIGH) {
    current_angle++;
  }
  else {
    current_angle--;
  }

  // Constrain the current angle between 0 and 180 degrees
  current_angle = constrain(current_angle, 0, 180);
}

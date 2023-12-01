// Define the digital input pins for the limit switches
#define LIM_SWITCH1 2
#define LIM_SWITCH2 3

// Define the pull-up resistor values (in Ohms)
#define PULLUP_RESISTOR 10000

// Define the distance between the two limit switches (in inches)
#define SWITCH_DISTANCE 5

// Define the number of steps per inch for the stepper motor
#define STEPS_PER_INCH 100

// Define the start position of the rack (in steps)
int start_position = 0;

void setup() {
  // Enable the internal pull-up resistors for the limit switch inputs
  pinMode(LIM_SWITCH1, INPUT_PULLUP);
  pinMode(LIM_SWITCH2, INPUT_PULLUP);
  
  // Set up the serial communication for debugging purposes
  Serial.begin(9600);
}

void loop() {
  // Read the state of the limit switches
  int switch1_state = digitalRead(LIM_SWITCH1);
  int switch2_state = digitalRead(LIM_SWITCH2);
  
  // Check if both limit switches are pressed
  if (switch1_state == LOW && switch2_state == LOW) {
    // Calculate the total travel distance of the rack (in steps)
    int total_distance = SWITCH_DISTANCE * STEPS_PER_INCH;
    
    // Move the rack until it contacts the first limit switch
    while (digitalRead(LIM_SWITCH1) == HIGH) {
      // Move the rack one step
      // (Note: Replace this with your own code to control the stepper motor)
      start_position++;
      delay(10);
    }
    
    // Record the current position as the start position
    start_position = 0;
    
    // Move the rack in the opposite direction until it contacts the second limit switch
    while (digitalRead(LIM_SWITCH2) == HIGH) {
      // Move the rack one step
      // (Note: Replace this with your own code to control the stepper motor)
      start_position--;
      delay(10);
    }
    
    // Calculate the start position based on the total travel distance and the position of the second switch
    start_position += total_distance / STEPS_PER_INCH;
    
    // Print the start position to the serial monitor
    Serial.print("Start Position: ");
    Serial.println(start_position);
  }
  
  // Add the rest of your code here
}
